const express = require("express");
const bodyParser = require("body-parser");
const url = require("url");
const querystring = require("querystring");

let app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

// Function to handle the root path
app.get("/", async function (req, res) {
  // Access the provided 'page' and 'limt' query parameters
  console.log(req.query);

  // Return the articles to the rendering engine
  res.send("GOOD");
});

let server = app.listen(3030, function () {
  console.log("Server is listening on port 3030");
});
