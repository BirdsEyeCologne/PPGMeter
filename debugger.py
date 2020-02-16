#!/usr/bin/python3

import bluetooth
import struct
import datetime

class SensorData:

	def __init__(self, data):
		""" Init SensorData from byte array containing data in format:
		   [float:  tempCht,
			float:  tempEgt,
			float:  tempAir,
			float:  pressureAir,
			uint16: rpmMotor,
			uint32: runtimeMotor]
		"""
		self._temp_cht = struct.unpack('>f', data[0:4])[0]
		self._temp_air = struct.unpack('>f', data[8:12])[0]
		self._preasure = struct.unpack('>f', data[12:16])[0]
		self._rpm = struct.unpack('>H', data[16:18])[0]
		self._runtime = struct.unpack('>I', data[18:22])[0]

	def __str__(self):
		return ('CHT:      {}°C\n'
				'Air:      {}°C\n'
				'Preasure: {}hPa\n'
				'RPM:      {}\n'
				'Hours:    {}'.format(
			self._temp_cht,
			self._temp_air,
			self._preasure,
			self._rpm,
			datetime.timedelta(seconds=self._runtime)))

class SensorDataReceiver:

	start_byte = 0xAC
	end_byte = 0xAD
	message_size = 22

	def __init__(self, socket):
		self._socket = socket

	def receive(self, max_sync=None):
		""" Receive data until start and end bytes match or until max_sync bytes where received. """
		data = b''
		print_sync = False
		sync_count = 0
		while True:
			data += sock.recv(1)
			if len(data) < self.message_size + 2:
				continue
			while len(data) > self.message_size + 2:
				sync_count += 1
				if (max_sync is not None) and (sync_count > max_sync):
					return None
				data = data[1:]
				if not print_sync:
					print('Syncing', end='')
				print('.', end='', flush=True)
				print_sync = True
			if (data[0] == self.start_byte) and (data[self.message_size + 1] == self.end_byte):
				if print_sync:
					print('')
				return SensorData(data[1:-1])

#nearby_devices = bluetooth.discover_devices(lookup_names=True)
#print("Found {} devices.".format(len(nearby_devices)))

#for addr, name in nearby_devices:
#	print("  {} - {}".format(addr, name))

uuid = '00001101-0000-1000-8000-00805F9B34FB'
address='98:D3:91:FD:5E:F5'
service_matches = bluetooth.find_service(uuid=uuid, address=address)
print(service_matches)

port = service_matches[0]["port"]
host = service_matches[0]["host"]
sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((host, port))

receiver = SensorDataReceiver(sock)
while True:
	data = receiver.receive(64)
	if data is None:
		print('Sync failed')
		break
	print()
	print(data)

sock.close()