console.log("hi");

const socket = io();
const container = document.getElementById("container");

socket.on("connect", () => {
  console.log(socket.id);
});

socket.on("rfid-list", (data) => {
  const children = data.reverse().map((d) => {
    const listItem = document.createElement("div");
    listItem.classList = ["rfid"];
    listItem.style = `background-color: ${
      d.status === "GOOD" ? "green" : "red"
    }`;
    listItem.innerHTML = `
      <div>RFID: ${d.rfid}</div>
      <div>id: ${d.id}</div>
      <div>status: ${d.status}</div>
    `;
    return listItem;
  });
  container.replaceChildren(...children);
});
