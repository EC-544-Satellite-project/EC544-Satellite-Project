Step 0 is a flow chart that shows the high level authentication method. From step 1 - 23, the steps of each authentication process is illustrated in comand line tool using openssl.


In this process, we implement the RSA key authentication method similar to the way we implement lab3, but we exchanged securID on top of the sender message. First, we generate 2 key pairs and 2 random hexadecimal 8 bytes passwords for B1 and B2. Then, both B1 and B2 encrypt their password with the counterpart's public key so that the counterpart can exchange passwords that are decryptable. 


B1 and B2 are also each given an 8-digit token to be used as its securID. Each securID is encrypted using its own public key and password so that, once either party has received the encrypted password from the counterpart and decrypted it, the identity of this counterpart can be authenticated. However, in real life, there is an additional procedure, which is to look up coumterpart's securID in the database so that the source of the message can be officially authenticated to be the right and trustworthy source. After both parties have exchanged passwords and securID, the whole authentication process is completed. During this process, encryption using aes-256-cbc is also used to ensure the sender's identity is protected.
