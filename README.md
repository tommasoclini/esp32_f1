# esp32_f1

## Una mini formula 1 controllata via espnow utilizzando un esp32

- ### Panoramica
  
  Questa e\` una macchinina formula uno in scala radiocomandata, il protocollo wireless usato e\` espnow, esso permette una comunicazione wireless senza connessione point to point, tra il telecomando e la macchinina ci deve essere lo scambio di questi tipi informazioni:
  
  - Tipo telecomando->macchinina:
    
    - Posizione accelleratore(power: [-1.0;1.0]) e sterzo(turn: [-1.0;1.0])
  
  - Tipo macchinina->telecomando:
    
    - Stato batteria(carica e temperatura)
    
    - Temperature varie(motore, esc, esp32, ecc.)
  
  - Tipo telecomando->macchinina di setup:
    
    - Offsets accelleratore e sterzo
