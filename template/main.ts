class Mage {
    x: number;
    y: number;
    width: number;
    height: number;
    image: string;
    collider: string;

    constructor(x: number, y: number) {
        this.x = x;
        this.y = y;

        this.width = 50;
        this.height = 50;
        this.image = turtle.graphics.newImage("assets/mage.png");
        this.collider = turtle.physics.newRectangleCollider(this.x, this.y, this.width, this.height);
    }

    update(dt: number) {
        this.x = turtle.physics.getX(this.collider);
        this.y = turtle.physics.getY(this.collider);
    }

    draw() {
        turtle.graphics.draw(this.image, this.x - 25, this.y - 40, 0, 4);
    }
}

class Ground {
    x: number;
    y: number;
    width: number;
    height: number;
    collider: string;

    constructor(x: number, y: number) {
        this.x = x;
        this.y = y;

        this.width = 800;
        this.height = 100;
        this.collider = turtle.physics.newRectangleCollider(this.x, this.y, this.width, this.height);
        turtle.physics.setType(this.collider, "static");
    }

    update(dt: number) {
        this.x = turtle.physics.getX(this.collider);
        this.y = turtle.physics.getY(this.collider);
    }

    draw() {
        turtle.graphics.rectangle("line", this.x - this.width / 2, this.y - this.height / 2, this.width, this.height);
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

let ground: Ground = new Ground(0, 500);
let mage: Mage = new Mage(100, 0);

let balls: Ball[] = [];

let camx = 0;
let camy = 0;

function update(dt)
{
    if (turtle.mouse.isPressed(0))
    {
        let newBall: Ball = new Ball(turtle.camera.toWorldX(turtle.mouse.getX()), turtle.camera.toWorldY(turtle.mouse.getY()));
        balls.push(newBall);
    }

    if (turtle.keyboard.isDown("up"))
    {
        camy -= 10;
    }
    else if (turtle.keyboard.isDown("down"))
    {
        camy += 10;
    }
    else if (turtle.keyboard.isDown("left"))
    {
        camx -= 10;
    }
    else if (turtle.keyboard.isDown("right"))
    {
        camx += 10;
    }

    turtle.camera.lookAt(camx, camy);

    for (let i = 0; i < balls.length; i++)
    {
        balls[i].update(dt);
    }

    ground.update(dt);
    mage.update(dt);

    if (turtle.physics.isColliding(ground.collider, mage.collider))
    {
        turtle.graphics.setBackgroundColor(turtle.math.random(0, 255), turtle.math.random(0, 255), turtle.math.random(0, 255), 255);
        turtle.graphics.setColor(turtle.math.random(0, 255), turtle.math.random(0, 255), turtle.math.random(0, 255), 255);
    }
}

function draw()
{
    turtle.camera.attach();

    ground.draw();
    mage.draw();

    turtle.graphics.rectangle("fill", turtle.camera.toWorldX(turtle.mouse.getX()), turtle.camera.toWorldY(turtle.mouse.getY()), 10, 10);

    for (let i = 0; i < balls.length; i++)
    {
        balls[i].draw();
    }

    turtle.camera.detach();

    turtle.graphics.print("Click to create balls!", 10, 10, 30);
    turtle.graphics.print("CAMX: " + camx.toString(), 10, 50, 30);
    turtle.graphics.print("CAMY: " + camy.toString(), 10, 80, 30);
}