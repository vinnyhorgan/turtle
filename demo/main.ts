let mod = require("mod");

let computer = turtle.audio.newSource("assets/computer.ogg");

function update(dt)
{
    if (turtle.keyboard.isPressed("return"))
    {
        turtle.audio.play(computer);
    }
}

function draw()
{

}