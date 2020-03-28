#include "packages.hpp"

BLECharacteristic nonce_chr = BLECharacteristic(nonce_uuid);
StreamCipher encryptor;

Package::Package(uint8_t _data_length, uint8_t type, bool _urgent){
    data_length = _data_length;
    urgent = _urgent;
    buffer[3] = type;

    length = data_length+4;//fix the lengths (probably should make a constructor)
    length_encrypted_part = ((data_length-1)/16+1)*16;
}

Package::Package(){
    acknowledged = true;
    data_length = 0;
}

uint8_t* Package::get_bufferpointer(){
    return &buffer[4];
}

uint32_t read_24_bit_int(uint8_t* source){
    uint32_t number = ((uint32_t)source[2]) << 16;
    number |= ((uint32_t)source[1]) << 8;
    number |= ((uint32_t)source[0]);
    return number;
}

//returns 24 bit package id
uint32_t Package::numb(){
    return read_24_bit_int(&buffer[0]);
}

//should works on the (stream)encrypted data this makes sure the hmac is
//unpredictable even if the same sensor values are send
void Package::update_hmac(){

    /*SHA1.beginHmac(HMAC_KEY, (int)sizeof(HMAC_KEY));
    SHA1.write(buffer, 20);
    SHA1.endHmac();
    SHA1.readBytes(hmac, 20);*/
}

//TODO fix encryption
void Package::encrypt(){
    //go through data in 16 byte blocks and encrypt
    //buffer is always a multiple of 16

    auto counter = encryptor.get_nonce_counter();
    write_24_bit_int(&buffer[0], counter);

    encryptor.encrypt(&buffer[3+0],16);
    encryptor.encrypt(&buffer[3+16],1);
}

void Package::finalise(){
    encrypt();
    update_hmac();
}

PackageQueue::PackageQueue()
 : data_chr(data_uuid), hmac_chr(hmac_uuid), awk_chr(awk_uuid){ //FIXME init list doesnt work, why? (temp fix in this::setup)
    for(int i=0; i<QUEUE_SIZE; i++){
        auto& package = queue[i];
        package.acknowledged = true;
    }
}

void PackageQueue::setup(){
    encryptor.init_from_key(KEY);
    uint8_t* nonce = encryptor.get_unencrypted_nonce();

    /*nonce_chr.setProperties(CHR_PROPS_READ); //write then get response
    nonce_chr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); //everybody may read, nobody write
    nonce_chr.setFixedLen(16);
    nonce_chr.begin();
    nonce_chr.write(nonce, 16);

    data_chr.setUuid(data_uuid); //fix for init list not working, should be temp
    data_chr.setProperties(CHR_PROPS_NOTIFY); //write then get response
    data_chr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); //everybody may read, nobody write
    data_chr.setFixedLen(20); //max 20 for notify, must be fixed so package type cant be recognised by size
    data_chr.setCccdWriteCallback(cccd_callback); //callbackthat fixes when someone starts or stops notify
    data_chr.begin();

    hmac_chr.setUuid(hmac_uuid); //fix for init list not working, should be temp
    hmac_chr.setProperties(CHR_PROPS_NOTIFY); //write then get response
    hmac_chr.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS); //everybody may read, nobody write
    hmac_chr.setFixedLen(20); //max 20 for notify
    hmac_chr.begin();

    awk_chr.setUuid(awk_uuid); //fix for init list not working, should be temp
    awk_chr.setProperties(CHR_PROPS_WRITE_WO_RESP); //write then get response
    awk_chr.setPermission(SECMODE_OPEN, SECMODE_OPEN); //everybody write no reading
    awk_chr.setWriteCallback(awk_callback);
    awk_chr.setFixedLen(3); //space for the packet id
    awk_chr.begin();*/
}

void PackageQueue::enqueue(Package&& to_add){
    to_add.last_notify = millis();
    //data_chr.notify(&to_add.buffer[0], 20);//to_add.length);
    //hmac_chr.notify(&to_add.hmac[0], HMAC_SIZE); 

    /*for(int i=0; i<QUEUE_SIZE; i++){
        auto& package = queue[i];
        if (package.acknowledged) {
            package = to_add;
            return;
        }
    }
    auto most_send_attempts = 0;
    auto best_to_replace = &queue[QUEUE_SIZE-1];
    for(int i=0; i<QUEUE_SIZE; i++){
        auto& package = queue[i];
        if (package.send_attempts>most_send_attempts){
            best_to_replace = &package;
        }
    }
    *best_to_replace = to_add;*/
}

void PackageQueue::poll(){
    auto now = millis();
    for(int i=0; i<QUEUE_SIZE; i++){
        auto& package = queue[i];
        //do not attempt to send packages that where already acknowledged
        if (package.acknowledged){continue;}
        
        if (package.urgent && now-package.last_notify > BURST_PERIOD) {
            
            //data_chr.notify(&package.buffer[0], 20);//.length);
            //hmac_chr.notify(&package.hmac[0], HMAC_SIZE);
            package.last_notify = now;

            if (package.send_attempts > BURST_SIZE-1){
                package.send_attempts=0;
                package.urgent = false;
                continue;                    
            } else {
                package.send_attempts++;
                continue;
            }
        } else if (now-package.last_notify > RESEND_PERIOD) {
            //data_chr.notify(&package.buffer[0], 20);//package.length);
            //hmac_chr.notify(&package.hmac[0], HMAC_SIZE);
            package.last_notify = now;
            package.send_attempts++;
            continue;
        }
    }
}

void PackageQueue::acknowledge(uint32_t package_numb){
    for(int i=0; i<QUEUE_SIZE; i++){
        auto& package = queue[i];
        if (package.numb() == package_numb){
            package.acknowledged = true;
        }
    }
}

void PackageQueue::reset(){
    for(int i=0; i<QUEUE_SIZE; i++){
        auto& package = queue[i];
        package.acknowledged = true;
    }
}

extern PackageQueue queue;
/*void awk_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len){
    //if (len!=3) {Serial.println("ERRROR AWK TO SHORT");}
    uint32_t package_numb = read_24_bit_int(data);
    queue.acknowledge(package_numb);
}*/ 

bool client_subscribed = false; //TODO move to main
/*void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value){

    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
    if (authentication::authenticated){
        if (chr->notifyEnabled(conn_hdl)) {
            delay(1000); //give central some time to get rdy to recieve notifications
            queue.awk_chr.setWriteCallback(awk_callback);
            client_subscribed = true;

            Serial.println("subscribed");
        } else {
            client_subscribed = false;
        }
    }
}*/