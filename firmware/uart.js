let serial = require("serial");
let textbox = require("textbox");

serial.setup("usart", 115200);
// textbox.setConfig("end", "text");
// textbox.clearText();
// textbox.show();

// let start = serial.readln();

// while (start !== undefined) start = serial.readln();

// print(start)

// let data = 0;
// data = (data << 8) | 255;
// data = (data << 8) | 1;
// data = (data << 4) | 1;
// data = (data << 4) | 1;

// serial.write(0);
// serial.write(255);
// serial.write(1);
// serial.write(1);
// serial.write(1);
while(1) {
// for (let i = 0; i < 14; i++) {
    let text = serial.readln(1000);
    while (!text) {
        text = serial.readln(1000);
    }
    print(text);
    delay(100);
}

delay(5000);
// let data = serial.readBytes(4, 10);
// let abc = Uint8Array(data);
// let typeOfProtocol = abc[2] & 15;
// let lenOfData = (abc[2] >> 4) & 15;
// print(to_hex_string(abc[0]) + '|' + to_hex_string(abc[1]) + '|' + to_hex_string(typeOfProtocol) + '|' + to_hex_string(lenOfData) + '|' + to_hex_string(abc[3]));

// print("Connected")

while (1) {
    let rx_data = serial.readBytes(3, 10);
    // let rx_data = serial.readln();
    if (rx_data !== undefined && Uint8Array(rx_data).buffer.byteLength > 2) {
        let abc = Uint8Array(rx_data);
        // print(abc[0], abc[1], abc[2]);
        let typeOfProtocol = abc[2] & 15;
        let lenOfData = (abc[2] >> 4) & 15;
        print(to_hex_string(abc[0]) + '|' + to_hex_string(abc[1]) + '|' + to_hex_string(typeOfProtocol) + '|' + to_hex_string(lenOfData));

        if (lenOfData !== undefined && lenOfData !== 0) {
            lenOfData += 1;
            rx_data = serial.readBytes(lenOfData, 10);
            if (rx_data !== undefined) {
                abc = Uint8Array(rx_data);

                if (typeOfProtocol === 6) {
                    // for (let i = 0; i < abc.buffer.byteLength; i++) {
                    //     print(i, ' - ', abc[i]);
                    // }
                    let speed = to_string((abc[0]) | ((abc[1] & 3) << 8));
                    let brake = to_string((abc[2]) | ((abc[3] & 3) << 8));
                    let left = to_string((abc[3] & 4) >> 2);
                    let right = to_string((abc[3] & 8) >> 3);
                    
                    print('speed: ' + speed)
                    print('brake: ' + brake)
                    print('left: ' + left)
                    print('right: ' + right)
                }
            }
        }
        // abc.forEach(element => {
        //     print(element)
        // });
        // print(to_string())
        // serial.write(rx_data);
        // let data_view = Uint8Array(rx_data);
        // print("0x" + to_hex_string(data_view[0]));
    }
}