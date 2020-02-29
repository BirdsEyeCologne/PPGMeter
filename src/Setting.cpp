#include "Setting.h"

// #######################
// # Add your types here #
// #######################

template<>
void Setting::Entry<std::string>::Parse(const std::string& value)
{
	Value = value;
}

template<>
void Setting::Entry<int>::Parse(const std::string& value)
{
	std::stringstream ss(value);
	ss >> Value;
}

template<>
void Setting::Entry<float>::Parse(const std::string& value)
{
	std::stringstream ss(value);
	ss >> Value;
}

template<>
void Setting::Entry<bool>::Parse(const std::string& value)
{
	Value = value == "1";
}

template<>
std::string Setting::Entry<int>::ToString()
{
	std::stringstream ss;
	ss << Value;
	return ss.str();
}

template<>
std::string Setting::Entry<float>::ToString()
{
	std::stringstream ss;
	ss << Value;
	return ss.str();
}

template<>
std::string Setting::Entry<bool>::ToString()
{
	return Value ? "1" : "0";
}

template<>
std::string Setting::Entry<std::string>::ToString()
{
	return Value;
}

// ###############################
// # Implementation of internals #
// ###############################

Setting& Setting::Instance()
{
	static Setting s;
	return s;
}

void Setting::Parse(const hash_t& identifier, const std::string& value)
{
	const iter_t i = Instance().mEntries.find(identifier);
	if(i == Instance().mEntries.end())
		ERROR("Setting was not found");
	i->second->Parse(value);
}

Setting::Iterator::Iterator(bool ordered)
	: mAt(Setting::Instance().mEntries.begin())
    , mOrdered(ordered)
    , mAtOrdered(-1)
{
    if(mOrdered)
        Next();
}

bool Setting::Iterator::Exists()
{
	return mAt != Setting::Instance().mEntries.end();
}

void Setting::Iterator::Next()
{
    if(mOrdered)
    {
        if(++mAtOrdered < Setting::Instance().mEntryOrder.size())
            mAt = Setting::Instance().mEntries.find(Setting::Instance().mEntryOrder[mAtOrdered]);
        else
            mAt = Setting::Instance().mEntries.end();
    }
    else
        ++mAt;
}

const std::string& Setting::Iterator::Name()
{
	return mAt->second->Name;
}

const char* Setting::Iterator::Identifier()
{
	return mAt->second->Identifier;
}

const hash_t& Setting::Iterator::Hash()
{
	return mAt->first;
}

std::string Setting::Iterator::ToString()
{
    return mAt->second->ToString();
}

void Setting::Load()
{
	/*File configFile = SPIFFS.open(SETTINGS_FILE, "r");
	if(configFile)
	{
		INFO("Opening setting file for reading ok");

		while(configFile.available() > 0)
		{
			String id = configFile.readStringUntil('=');
			String value = configFile.readStringUntil('\n');
			for(Iterator i; i.Exists(); i.Next())
				if(strcmp(i.Identifier(), id.c_str()) == 0)
				{
					i.mAt->second->Parse(value);
					break;
				}
		}

		Instance.mLoaded = true;
	}
	else
		ERROR("Opening setting file for reading failed");*/
}

void Setting::Save()
{
    /*File configFile = SPIFFS.open(SETTINGS_FILE, "w");
    if(configFile)
    {
        INFO("Opening setting files for saving ok");
		for(Iterator i; i.Exists(); i.Next())
		{
			configFile.print(i.Identifier());
			configFile.print('=');
			configFile.print(i.ToString());
			configFile.print('\n');
		}
		configFile.close();
    }
    else
        ERROR("Opening setting files for saving failed");*/
}

bool Setting::Loaded()
{
	return Instance().mLoaded;
}
