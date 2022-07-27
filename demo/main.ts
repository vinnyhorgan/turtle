let mod = require("mod");

mod.test();

let server = turtle.network.newServer("127.0.0.1", 3000);

console.log("Server started on port 3000...");

function update(dt)
{
    let event = turtle.network.service(server, 0);

    if (event.type == "connect")
    {
        console.log("RECEIVED NEW CONNECTION")
    }
    else if (event.type == "disconnect")
    {
        console.log("SOMEBODY DISCONNECTED");
    }
    else if (event.type == "receive")
    {
        console.log("Received: " + event.data);

        turtle.network.send(event.peer, "Thanks!", "reliable");
    }
}

function draw()
{

}