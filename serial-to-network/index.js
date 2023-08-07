import { SerialPort, ReadlineParser } from "serialport";
const port = new SerialPort({
  path: "/dev/cu.usbmodem31101",
  baudRate: 115200,
});
const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

// Read the port data
port.on("open", () => {
  console.log("serial port open");
});
parser.on("data", (data) => {
  console.log("got word from arduino:", data);
});
