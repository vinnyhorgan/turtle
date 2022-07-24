class Mage {
    x: number;
    y: number;
    image: string;
    collider: string;

    constructor(x: number, y: number) {
        this.x = x;
        this.y = y;

        this.image = turtle.graphics.newImage("assets/mage.png");
        this.collider = turtle.physics.newRectangleCollider(this.x, this.y, 50, 50);
    }

    update(dt: number) {
        this.x = turtle.physics.getX(this.collider);
        this.y = turtle.physics.getY(this.collider);
    }

    draw() {
        turtle.graphics.draw(this.image, this.x - 25, this.y - 40, 0, 4);
    }
}

class Ball {
    x: number;
    y: number;
    collider: string;

    constructor(x: number, y: number) {
        this.x = x;
        this.y = y;

        this.collider = turtle.physics.newCircleCollider(this.x, this.y, 20);
    }

    update(dt: number) {
        this.x = turtle.physics.getX(this.collider);
        this.y = turtle.physics.getY(this.collider);
    }

    draw() {
        turtle.graphics.circle("line", this.x, this.y, 20);
    }
}

let mage: Mage = new Mage(100, 100);

let ground: string = turtle.physics.newRectangleCollider(10, 400, 600, 100);
turtle.physics.setType(ground, "static");

let balls: Ball[] = [];

function update(dt)
{
    mage.update(dt);

    if (turtle.mouse.isPressed(0))
    {
        let newBall: Ball = new Ball(turtle.mouse.getX(), turtle.mouse.getY());
        balls.push(newBall);
    }

    for (let i = 0; i < balls.length; i++)
    {
        balls[i].update(dt);
    }
}

function draw()
{
    mage.draw();

    turtle.graphics.rectangle("line", 10 - 300, 400 - 50, 600, 100);

    for (let i = 0; i < balls.length; i++)
    {
        balls[i].draw();
    }

    turtle.graphics.print("Click to create balls!", 10, 10, 30);
}