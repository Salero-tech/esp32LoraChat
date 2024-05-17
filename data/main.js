const Sources = {
    me: 'me',
    lora: 'lora',
  };

//loraSendUrl = "/api/lorasend";
//loraRecUrl = "/api/lorarec"
loraSendUrl = "/api/lorasend";
loraRecUrl = "/api/lorarec"

//scroll
var scrollPosOld = 0;
var scrollOldmax = 0;
var scrollLock = false;
function scroll () {
    const scrollDiv = document.getElementById("chatbox");
    const scrollPositon = scrollDiv.scrollTop;
    const maxScroll = scrollDiv.scrollHeight;

    console.log(scrollPositon, scrollOldmax, maxScroll);
    if (scrollPositon < scrollPosOld) scrollLock = true;

    scrollPosOld = scrollPositon;
    scrollOldmax = maxScroll;

    if (scrollLock) return;
    scrollDiv.scrollTo(0,maxScroll);
}

function send () {
    //get msg
    const msg = document.getElementById("loraSMG").value;
    console.log(msg);

    toChatBox(msg, Sources.me);
    sendLora(msg);
}

//intefal for data rec
window.setInterval(getRec, 500);

async function getRec () {
    
    msg = await getLora();

    if (msg == "") return;

    console.log(msg);

    toChatBox(msg, Sources.lora);
}

function toChatBox (msg, source) {
    const chatBox = document.getElementById("chatbox");
    
    const mainTag = document.createElement("div");
    const msgTag = document.createElement("p");
    const authorTag = document.createElement("p");

    const text = document.createTextNode(msg);
    const authorText = document.createTextNode("");

    //classes
    mainTag.className = "msg";
    authorTag.className = "author";
    msgTag.className = "msgText";
    //build
    switch (source){
        case Sources.me:
            mainTag.className += " fromMe";
            authorText.data = "YOU";
            break;

        case Sources.lora:
            mainTag.className += " fromLora";
            authorText.data = "LORA";
            break;
    }
    
    msgTag.appendChild(text);
    authorTag.appendChild(authorText);
    mainTag.appendChild(authorTag);
    mainTag.appendChild(msgTag);
    chatBox.appendChild(mainTag);

    scroll();
}


async function sendLora (msg) {
    //send
    resp = await fetch(loraSendUrl, {
        method: 'POST',
        mode: 'no-cors',
        body: msg,
    });
}

async function getLora () {
    //send
    resp = await fetch(loraRecUrl);

    const jsonData = await resp.text();
    return jsonData;
}