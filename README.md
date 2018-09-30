# demo nonboost asio multithreaded ssl/tls client


# Protokoll leírás

## A protokoll feladata:

Ez a dokumentum tartalmazza egy elképzelt projekt keretében megvalósuló szerver-kliens architektúrájú, tetszőleges feladatokat végző rendszer kommunikációs leírását. Ezen leírás szerint  meg lehet valósítani minden kommunikációt a szerver és kliens, valamint kliens és kliens között.

## A szerver feladata:

• A szerver tárolja azokat a feladatokat (továbbiakban job-okat), amelyeket az egyes klienseknek el kell végezniük. A job maga bármit tartalmazhat (részletesen lásd jobleíró szakasz), struktúrája kötött ám tartalma tetszőleges. 
    
• A szerver feladata autentikálni a klienst. A kommunikáció során a kliens mindig csak jelzéseket, kéréseket fogalmaz meg a szerver felé, de annak eldöntése, hogy ezeket teljesíti-e teljes mértékben a szerver hatásköre. A jobok esetében ez azt jelenti, hogy a kliens jelzi, hogy készen áll egy feladat fogadására és végrehajtására (közölve, hogy milyen erőforrásokkal rendelkezik). 
    
• A szerver feladata ezek után kiválasztani a következőként elvégzendő feladatot. 

• A szerver valamennyi eseményt naplóz.

## A kliens feladata:

• Felvenni a kapcsolatot a szerverrel

• Autentikálni magát

• Autentikálni a szervert

• Jelezni ha készen áll egy feladat végrehajtására egyben közölve saját erőforrásait (a szerver ez alapján dönt, mi legyen a következő elvégzendő feladat)

• Végrehajtani a szervertől érkező feladatokat, vezérlő utasításokat, tekintet nélkül annak tartalmára. Ez alól kivétel, hogyha egy feladat elvégzéséhez nem rendelkezik a kellő erőforrásokkal, előkövetelményekkel és azok kielégítéséről sem képes gondoskodni. Ekkor a feladat végrehajtása nélkül hibaüzenettel értesíti a szervert.

• Válaszolni a szervertől érkező kérésekre (pl. erőforrások aktuális állapota)

• Közölni, ha egy feladat befejeződött (hiba nélkül vagy hibával)

• Naplózni valamennyi eseményt

• Egyszerre több feladatot kell tudnia elvégezni

## A kommunikációs csatorna (socket)

A kommunikáció a szerver és a kliens között TCP/IP protokoll használatával megvalósuló SSL/TLS (Secure Sockets Layer/Transport Layer Security) használatával történik. Ennek oka, hogy az eljárás a legtöbb programozási nyelvben implementált (köszönhetően az OpenSSL-nek), megfelelően biztonságos kommunikációt tesz lehetővé és segítségével a kliens azonosítani tudja a szervert (a szerver ezen eljárással a klienst viszont nem! Részletesen lásd autentikációs szakasz). A titkosítás így egyszerűen, a felsőbb rétegek számára rejtett módon, egységbe zárva (encapsulated) valósul meg, hiszen a titkosítás/szerver azonosítás socket szinten történik.

## A kapcsolat felépítése, szerver autentikálása

A szerver és kliens a kapcsolat felépítésekor az SSL/TLS protokoll leírásában definiált módon létesítenek kapcsolatot. A preferált mód a legfrissebb (ezen dokumentum születésekor az RFC 5246 szabványban leírt v1.2) TLS. A kapcsolat felépítésekor a szabvány szerint a szerver elküldi tanúsítványát, amelyet a kliens ellenőriz. Ha a tanúsítvány nem megfelelő bontja a kapcsolatot. Ha megfelelő akkor megkezdődhet a kommunikáció. Egy kliens a szerverhez egy szálon kapcsolódik, tekintet nélkül arra, hogy a klienst hány szálú működésre állították be.

## A kliens autentikálása

Ha a kliens meggyőződött róla, hogy a szerverrel vette fel a kapcsolatot, akkor autentikálja magát. Ehhez az üzenetben elküldi saját FQDN-jét, a Unix epoch óta eltelt másodpercek számát, valamint aláírja az üzenetet a saját titkos kulcsával (a nyilvános kulcsnak a szerveren rendelkezésre kell állnia). Az időbélyeg azt a célt szolgálja, hogy az üzenet tartalma ne legyen ugyanaz valamennyi autentikációkor, mert az biztonsági kockázatot rejtene. Az RSA algoritmus futtatása -eltérően a széles körben elterjedt gyakorlattól- az üzeneten, nem pedig annak ujjlenyomatán történik. Ennek oka, hogy az üzenet hossza rendkívül kicsi (Az hash hosszabb lenne, mint maga az üzenet). A szerver válaszüzenetben nyugtázza, hogy megkapta az üzenetet, elvégzi az aláírás ellenőrzését, majd ha az aláírás helyes akkor visszajelez egy másik üzenetben, hogy az azonosítás rendben megtörtént. Ha az aláírás helytelen, bontja a kapcsolatot.

## Üzenetváltások áttekintése

A kommunikációs csatorna felépítése után kezdődhetnek az üzenetváltások. Minden üzenet nyugtázása kétszeresen történik. Először az üzenet vétele után, de értelmezése előtt vételi nyugta kerül küldésre. Ha az üzenet értelmezése is rendben megtörtént értelmezési nyugtát küld a fogadó fél. (A továbbiakban ezen nyugták küldését külön nem említjük.)
Az első valódi üzenetet a kliens küldi jelezve, hogy kész a feladatok végrehajtására. A szerver ez után egy feladatot küld a kliensnek. A kliens a feladatot értelmezi és megkezdi a végrehajtását. Ha a klienset többszálú feladat-végrehajtásra állították be, akkor nem várja meg a feladat befejezését, hanem egy újabb jobkérő kérést küld a szervernek. 
Ha egy feladatot befejezett vagy a futtatás során hiba lépett fel, akkor arról üzenetet küld a szervernek mellékelve az esetleges hibaüzenetet.
Ezek után a képességfelmérő script (megfelelő feladatait) újra lefuttatja, új üzenetben új feladatot kér mellékelve az aktuális képességeket.

Az üzenettípusok egyik fontos attribútuma, hogy várunk-e rá válaszüzenetet (természetesen a nyugtákon kívül). Az egyes üzenetek részletes leírását az Üzenettípusok szakasz tartalmazza.
Ha épp nincs olyan job, amit a kliens képes végrehajtani, akkor a szerver várakozásra utasíthatja a klienst.
Ez a normál szekvencia, de a szerver bármikor külön kérdést/utasítást is megfogalmazhat.
A szerver vezérlő utasításokat is küldhet a kliensnek. Ez abban különbözik egy jobtól, hogy kifejezetten a kliens paramétereinek beállítására szolgál (például párhuzamos szálak száma, naplózás módja, naplózás helye).


## Üzenet szerkezete

Az üzenet HEADER-BODY struktúrájú.
A HEADER tartalma: 

• [bodylenght]: A BODY hossza byte-ban

Típusa: unsigned long long integer

Hossza: 8 byte

• [sender]: üzenet feladója: A küldő FQDN-je

Típusa: string(ekkor ki kell tölteni az összes helyet) vagy char [31] (ekkor nem kell kitöltő 	karakter)

Hossza: 64 byte (511 karakter + záro 0)

• [seq]: üzenet sorszáma. Ez egy számláló. Kezdő értéke a session indulásakor véletlen szám, majd minden üzenet előállítása után értéke 1-el növekszik. 

Típusa: unsigned int

Hossza: 4byte

• [response_to]: az üzenet azonosítója, amire válaszol. Ha az üzenetválaszüzenet, akkor itt tartalmazza annak az üzenetnek az azonosítóját, amire válaszol. Ha nem válasz, akkor értéke 0.

Típusa: unsigned int

Hossza: 4 byte

• [type]: az üzenet enum típusa. 

Típusa: unsigned int

Hossza: 4 byte

      
      
• [full_path]: BODY puffer helye abszolút path-al

Típusa: string/char[3423] (lásd [sender])

Hossza: 428 byte (3423 karakter + záro 0)

Alapértelmezett értéke: „default” ekkor átmenetileg a konfigban szereplő helyre kerül kiírásra a BODY tartalma, majd a feldolgozás után törlésre kerül.

A HEADER hossza így fix 512 byte:

Byte térkép:

|  Név  | body lenght  | sender | seq | response_to | type | full_path |
| ----- | ------------ | ------ | --- | ----------- | ---- | --------- |
| Hossz |       8      |   64   |  4  |      4      |   4  |    428    |



A BODY tartalma:
• üzenettípus függő a tartalom

Típusa: byte array

Hossza: nem nagyobb, mint 2^64 byte
