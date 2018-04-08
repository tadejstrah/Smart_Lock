ECC - Elliptic-curve cryptography 

All of the functions are ASYNC, use Promises as shown in test.html .

Available functions; 
encrypt2json = function(publicKey, text){
  //Takes in publicKey(buffer), text(string) and returns promise with encrypted data in JSON
  text = Buffer(text)
  return ecc.encrypt(publicKey, text).then(function(response){
    return JSON.stringify(response)
  })
}

decrypt2string = function(privateKey,json_encrypted){
  //Takes in JSON, privateKey(byte buffer) and returns decrypted text(string).
  json_encrypted = JSON.parse(json_encrypted)
  for (var key in json_encrypted){
    json_encrypted[key] = new Buffer(json_encrypted[key]['data']) 
  }
  return ecc.decrypt(privateKey, json_encrypted).then(function(decryptedText){
    return decryptedText.toString('UTF-8')
})}

genPrivateKey = function(){
  //Returns a buffer of 32 random bytes.
  return crypto.randomBytes(32)
}

genPublicKey = function(privateKey){
  //Genereates a public key pair from private key(byte buffer).
  return ecc.getPublic(privateKey)
}

buffer2base64 = function(rawdata){
  //Converts byte buffer to base64 format
  return rawdata.toString('base64')
}

base642buffer = function(base64){
  //Converts base64 to byte buffer.
  return Buffer.from(base64, 'base64')
}

sign = function(privateKey, msg){
  //Takes in private key(byte buffer) and message(string), returns digital signature in byte buffer.
  msg = crypto.createHash("sha256").update(msg).digest()
  return ecc.sign(privateKey, msg).then(function(sig) {
    return sig})
}

verify = function(publicKey, msg, sig){
  //Takes in public key(byte buffer), message(string) and signature(byte buffer),
  // returns True if signature is valid, False otherwise
  msg = crypto.createHash("sha256").update(msg).digest()
  
  return ecc.verify(publicKey, msg, sig).then(function() {
    console.log("Signature is OK");
    return true}).catch(function() {console.log("Signature is BAD\n")
    return false})
}