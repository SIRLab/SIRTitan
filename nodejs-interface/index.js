"use strict";
const serialport = require("serialport");
const stdin = process.openStdin();
const initStdin = () => {
	stdin.setRawMode(true);
	stdin.resume();
	stdin.setEncoding('utf8');
};
const joystickMap = { 6: 'W', 4: 'A', 7: 'S', 5: 'D', 0: '3', 1: '2', 2: '1' };
let explorer = null;
let currentJsKey = null;
let explorerPort = null;

console.log('   _____   _          _______   _   _                   ');
console.log('  / ____| (_)        |__   __| (_) | |                  ');
console.log(' | (___    _   _ __     | |     _  | |_    __ _   _ __  ');
console.log('  \\___ \\  | | | \'__|    | |    | | | __|  / _` | | \'_ \\ ');
console.log('  ____) | | | | |       | |    | | | |_  | (_| | | | | |');
console.log(' |_____/  |_| |_|       |_|    |_|  \\__|  \\__,_| |_| |_|');
console.log('');
console.log("Localizing XBee Explorer port");

serialport.list((err, ports) => {
	explorer = ports.filter((port) => port.manufacturer === 'FTDI')[0];
	if (explorer) {
		onExplorerFound();
	} else {
		console.log("XBee Explorer not found, exiting...");
		process.exit();
	}
});

const onExplorerFound = () => {
	explorerPort = new serialport(explorer.comName);
	explorerPort.on('open', () => {
		console.log("Connection established with the XBee Explorer!");
		console.log("Finding Joystick...");
		findJoystrick();
	});
	explorerPort.on('error', (err) => {
		console.log('XBee Explorer raised an error while connection: ' + err.message);
		console.log('Now exiting...');
		process.exit();
	});
};

const findJoystrick = () => {
	const jsport = new serialport('/dev/input/js0');
	jsport.on('open', () => {
		console.log("Joystick found!");
		jsport.on('data', (data) => {
			if (data.length === 8 && joystickMap[data[7]]) {
				currentJsKey = data[4] === 1 ? joystickMap[data[7]] : null;
				sendJoystickData();
			}
		});
	});
	jsport.on('error', (err) => {
		console.log("Joystick not found, error raised: " + err.message);
		console.log("Switching to keyboard mode...");
		initStdin();
		stdin.on('data', (data) => {
			if (data === '\u0003') {
				process.exit();
				return;
			}
			sendData(data);
		});
	});
	jsport.on('disconnect', (err) => {
		console.log('Joystick disconnect, message: ' + err.message);
		console.log('Now exiting...');
		process.exit();
	});
};

const sendJoystickData = () => {
	if (currentJsKey) {
		sendData(currentJsKey);
		setTimeout(sendJoystickData, 100);
	}
}

const sendData = (data) => {
	if (explorerPort === null) return;
	explorerPort.write(data, (err) => {
		if (err) {
			return console.log('Error on data sending: ' + err.message);
		}
		console.log('Data sent: ' + data);
	});
};