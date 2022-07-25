declare namespace turtle {
    namespace audio {
        function newSource(filename: string): string;
        function setMasterVolume(volume: number): void;
        function play(sound: string): void;
        function stop(sound: string): void;
        function pause(sound: string): void;
        function resume(sound: string): void;
        function isPlaying(sound: string): boolean;
        function setVolume(sound: string, volume: number): void;
        function setPitch(sound: string, pitch: number): void;
    }

    namespace camera {
        function create(): string;
        function attach(): void;
        function detach(): void;
        function lookAt(x: number, y: number): void;
        function zoom(zoom: number): void;
        function rotate(rotation: number): void;
        function toWorldX(x: number): number;
        function toWorldY(y: number): number;
        function getX(): number;
        function getY(): number;
        function getZoom(): number;
        function getRotation(): number;
    }

    namespace filesystem {

    }

    namespace graphics {
        function print(text: string, x: number, y: number, size: number): void;
        function circle(mode: string, x: number, y: number, radius: number): void;
        function draw(image: string, x: number, y: number, rotation: number, scale: number): void;
        function ellipse(mode: string, x: number, y: number, radiusX: number, radiusY: number): void;
        function line(x1: number, y1: number, x2: number, y2: number): void;
        function point(x: number, y: number): void;
        function rectangle(mode: string, x: number, y: number, width: number, height: number): void;
        function triangle(mode: string, x1: number, y1: number, x2: number, y2: number, x3: number, y3: number,): void;
        function newImage(filename: string): string;
        function newFont(filename: string): string;
        function captureScreenshot(filename: string): void;
        function setBackgroundColor(r: number, g: number, b: number, a: number): void;
        function setColor(r: number, g: number, b: number, a: number): void;
        function setFont(font: string);
    }

    namespace keyboard {
        function isDown(key: string): boolean;
        function isPressed(key: string): boolean;
        function isReleased(key: string): boolean;
    }

    namespace math {
        function random(min: number, max: number): number;
        function setRandomSeed(seed: number): void;
    }

    namespace mouse {
        function isDown(button: number): boolean;
        function isPressed(button: number): boolean;
        function isReleased(button: number): boolean;
        function getX(): number;
        function getY(): number;
        function getWheelMove(): number;
        function setGrabbed(grabbed: boolean): void;
        function isGrabbed(): boolean;
        function setVisible(visible: boolean): void;
        function isVisible(): boolean;
    }

    namespace physics {
        function newCircleCollider(x: number, y: number, radius: number): string;
        function newRectangleCollider(x: number, y: number, width: number, height: number): string;
        function getX(collider: string): number;
        function getY(collider: string): number;
        function getType(collider: string): string;
        function getMass(collider: string): number;
        function getFriction(collider: string): number;
        function setType(collider: string, type: string): void;
        function setX(collider: string, x: number): void;
        function setY(collider: string, y: number): void;
        function setMass(collider: string, mass: number): void;
        function setFriction(collider: string, friction: number): void;
        function setCollisionClass(collider: string, collisionClass: string): void;
        function isColliding(collider1: string, collider2: string): boolean;
    }

    namespace system {
        function getClipboardText(): string;
        function setClipboardText(text: string): void;
        function getOS(): string;
        function openURL(url: string): void;
    }

    namespace timer {
        function getDelta(): number;
        function getFPS(): number;
        function getTime(): number;
    }

    namespace window {
        function close(): void;
        function getDisplayWidth(): number;
        function getDisplayHeight(): number;
        function getWidth(): number;
        function getHeight(): number;
        function getDisplayName(): string;
        function getFullscreen(): boolean;
        function getX(): number;
        function getY(): number;
        function getTitle(): string;
        function getVSync(): boolean;
        function hasFocus(): boolean;
        function isVisible(): boolean;
        function isMaximized(): boolean;
        function isMinimized(): boolean;
        function maximize(): void;
        function minimize(): void;
        function restore(): void;
        function setFullscreen(fullscreen: boolean): void;
        function setPosition(x: number, y: number): void;
        function setTitle(title: string): void;
        function setVSync(vSync: boolean): void;
        function setResizable(resizable: boolean): void;
        function isResized(): boolean;
        function setMinSize(width: number, height: number): void;
    }
}