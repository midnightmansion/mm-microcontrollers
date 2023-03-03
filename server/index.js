const express = require("express");
const bodyParser = require("body-parser");
const url = require("url");
const querystring = require("querystring");
const path = require("path");

const SocketIO = require("socket.io");
const http = require("http");

const app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

const MAX_MEMORY = 10;
const IN_MEMORY_DATABASE = [];
const BAD_RFIDS = ["84DBBD5A"];

// Function to handle the root path
app.get("/", async function (req, res) {
  // Access the provided 'page' and 'limt' query parameters
  const params = req.query;
  if (Object.hasOwn(params, "id") && Object.hasOwn(params, "rfid")) {
    if (BAD_RFIDS.includes(params.rfid)) {
      res.send("BAD*");
      params.status = "BAD";
    } else {
      res.send("GOOD*");
      params.status = "GOOD";
    }
    if (IN_MEMORY_DATABASE.length >= MAX_MEMORY) {
      IN_MEMORY_DATABASE.shift();
    }
    IN_MEMORY_DATABASE.push(params);
    sendRFID();
  } else {
    res.send("BAD*");
  }
});

// Serve the static files from the React app
app.use(express.static(path.join(__dirname, "./public")));

function onError(error) {
  if (error.syscall !== "listen") {
    throw error;
  }

  const bind = typeof port === "string" ? "Pipe " + port : "Port " + port;

  // handle specific listen errors with friendly messages
  switch (error.code) {
    case "EACCES":
      console.error(bind + " requires elevated privileges");
      process.exit(1);
      break;
    case "EADDRINUSE":
      console.error(bind + " is already in use");
      process.exit(1);
      break;
    default:
      throw error;
  }
}

function onListening() {
  const addr = server.address();
  const bind = typeof addr === "string" ? "pipe " + addr : "port " + addr.port;
  console.info("Listening on port", bind);
}

function handleIo(io) {
  io.on("connection", (socket) => {
    console.log("connected", socket.id);

    socket.on("disconnect", () => {
      console.log("disconnected", socket.id);
    });
  });
}

const port = 3030;
const server = http.createServer(app);
server.on("error", onError);
server.on("listening", onListening);
server.listen(port);

const io = SocketIO(server);
function sendRFID() {
  io.emit("rfid-list", IN_MEMORY_DATABASE);
}
handleIo(io);
