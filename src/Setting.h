#pragma once

#include <Global.h>

#include <unordered_map>
#include <vector>
#include <sstream>


// Compile time hashing from string to int
typedef uint16_t hash_t;
constexpr hash_t SHIFT(hash_t h)
{
    return (h << 1) | (1 & (h >> (sizeof(hash_t) - 1)));
}
constexpr hash_t H(const char* str)
{
	return (*str) ? SHIFT(H(str + 1) ^ (*str)) : 0;
}

/// Setting database, add settings, quickly access them, save and load state to an ini file
/**
 * All public methods are static because there is one static centralized database.
 **/
class Setting
{
public:

	/// Add a setting with a type name, identifier and default value
	/**
	 * The name is the human readable name that is used in the ini file when saving the database.
	 * The identifier is a string that is hashed to an integer at compile time and used for getting and
	 * setting the value of the setting in time critical portions of your code. It must not contain
	 * controll charactes or the character =.
	 * The default value is the inital value that overwritten when tha database is successfully loaded.
	 * 
	 * See documentation of Setting::Entry to know which types are supported as TYPE.
	 **/
	template<typename TYPE>
	static void Add(const std::string& name, const char* identifier, const TYPE& defaultValue);

	/// Get current value of a setting by it's identifier, to hash an identifier write H("identifier")
	template<typename TYPE>
	static const TYPE& Get(const hash_t& identifier);

	/// Set current value of a setting, this does not automatically persist the database
	template<typename TYPE>
	static void Set(const hash_t& identifier, const TYPE& value);

	/// Set current value of a setting by parsing it's value readable string value
	static void Parse(const hash_t& identifier, const std::string& value);

	/// Load database from SETTINGS_FILE
	static void Load();

	/// Save database to SETTINGS_FILE
	static void Save();

	/// Check whether the  database was successfully loaded from SETTINGS_FILE
	static bool Loaded();

private:

	Setting()
	: mLoaded(false)
	{}

	static Setting& Instance();

	struct EntryBase
	{
		std::string Name;
		const char* Identifier;
		virtual void Parse(const std::string& value) = 0;
		virtual std::string ToString() = 0;
	};

	/// All settings in the database will be stored inside this class
	template<typename TYPE>
	struct Entry : public EntryBase
	{
		/// The actual current value of a setting
		TYPE Value;
		/// Convert a human readable string into TYPE
		/**
		 * All types that should be supported must provide an implementation of this function in Setting.cpp.
		 * The parsed string must be in the format generated by ToString().
		 * 
		 * Example for int:
		 * template<> void Setting::Entry<int>::Parse(const String& value) { std::stringstream ss(value); ss >> Value; }
		**/
		void Parse(const std::string& value);
		/// Convert TYPE into a human readable string
		/**
		 * All types that should be supported must provide an implementation of this function in Setting.cpp.
		 * The generated string must not contain controll characters.
		 * 
		 * Example for int:
		 * template<> String Setting::Entry<int>::ToString() { std::stringstream ss; ss << Value; return ss.str(); }
		**/
		std::string ToString();
	};

	std::unordered_map<hash_t, EntryBase*> mEntries;
	typedef std::unordered_map<hash_t, EntryBase*>::iterator iter_t;
    std::vector<hash_t> mEntryOrder;

    bool mLoaded;

public:

	/// Iterator class that iterates over all settings in the database
	class Iterator
	{
	public:

		/// Create iterator orderd by insertion order (true) or hash (false)
		Iterator(bool ordered = false);

		bool Exists();

		void Next();

		const std::string& Name();

		const char* Identifier();

		std::string ToString();

		const hash_t& Hash();

	private:
		iter_t mAt;
        friend class Setting;

        bool mOrdered;
        size_t mAtOrdered;
	};
};

// ###############################
// # Implementation of internals #
// ###############################

template<typename TYPE>
void Setting::Add(const std::string& name, const char* identifier, const TYPE& defaultValue)
{
	hash_t hash = H(identifier);
	if(Instance().mEntries.find(hash) != Instance().mEntries.end())
	{
		ERROR("Did not add setting %s, identifier already existed as %s.", name.c_str(),
		    Instance.mEntries.find(hash)->second->Name.c_str());
	}
	else
	{
		Entry<TYPE>* e = new Entry<TYPE>();
		e->Name = name;
		e->Identifier = identifier;
		e->Value = defaultValue;
		Instance().mEntries[hash] = e;
        Instance().mEntryOrder.push_back(hash);
	}
}

template<typename TYPE>
const TYPE& Setting::Get(const hash_t& identifier)
{
	const iter_t i = Instance().mEntries.find(identifier);
	if(i == Instance().mEntries.end())
		ERROR("Setting was not found");
	return ((Entry<TYPE>*)i->second)->Value;
}

template<typename TYPE>
void Setting::Set(const hash_t& identifier, const TYPE& value)
{
	const iter_t i = Instance().mEntries.find(identifier);
	if(i == Instance().mEntries.end())
		ERROR("Setting was not found");
	((Entry<TYPE>*)i->second)->Value = value;
}