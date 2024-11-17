const http = require("http");
const WebSocket = require("ws");
const fs = require("fs");
const path = require("path");
const os = require("os");

const port = process.env.PORT || 5000;
const server = http.createServer();
const wss = new WebSocket.Server({ server });

// Fonction pour obtenir l'adresse IP du serveur
function getServerIP() {
  const networkInterfaces = os.networkInterfaces();
  for (const interfaceName in networkInterfaces) {
    const interfaces = networkInterfaces[interfaceName];
    for (const interface of interfaces) {
      if (!interface.internal && interface.family === "IPv4") {
        return interface.address;
      }
    }
  }
  return "Adresse IP non trouvée";
}

wss.on("connection", function connection(ws) {
  console.log("Nouvelle connexion WebSocket établie.");

  /*ws.on("message", function incoming(message) {
    console.log("Message reçu depuis ESP32:", message);
    // Traitez le message reçu ici, si nécessaire
    if(message.includes("cm")){
      Distance(message);
    } else if(message.inc){"cm/s"}
     updateSpeed(message);
  });*/

  // Vous pouvez également envoyer un message à l'ESP32 après la connexion
  ws.send("Bienvenue !");
});

server.on("request", (req, res) => {
  // Servir le fichier index.html lorsqu'une requête HTTP est reçue
  if (req.url === "/" || req.url === "/index.html") {
    const filePath = path.join(__dirname, "index.html");
    fs.readFile(filePath, "utf-8", (err, data) => {
      if (err) {
        res.writeHead(500);
        res.end("Erreur interne du serveur");
        return;
      }
      res.writeHead(200, { "Content-Type": "text/html" });
      res.end(data);
    });
  } else if (req.url === "/styles.css") { // Ajout de cette condition pour servir le fichier CSS
    const filePath = path.join(__dirname, "styles.css");
    fs.readFile(filePath, "utf-8", (err, data) => {
      if (err) {
        res.writeHead(500);
        res.end("Erreur interne du serveur");
        return;
      }
      res.writeHead(200, { "Content-Type": "text/css" });
      res.end(data);
    });
    
  } else if (req.url === "/script.js") { // Ajout de cette condition pour servir le fichier JavaScript
    const filePath = path.join(__dirname, "script.js");
    fs.readFile(filePath, "utf-8", (err, data) => {
      if (err) {
        res.writeHead(500);
        res.end("Erreur interne du serveur");
        return;
      }
      res.writeHead(200, { "Content-Type": "text/javascript" });
      res.end(data);
    });
  } 
   else if (req.method === "POST" && req.url === "/direction") {
    let body = "";
    req.on("data", (chunk) => {
      body += chunk.toString(); // Convertit le chunk en chaîne de caractères
    });
    req.on("end", () => {
      const direction = body;
      console.log("Direction reçue :", direction);

      // Envoyer la direction à l'ESP32 via WebSocket
      wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
          client.send(direction);
        }
      });

      res.writeHead(200, { "Content-Type": "text/plain" });
      res.end("Direction envoyée avec succès à l'ESP32 !");
    });
  } else {
    // Autres routes peuvent être gérées ici
    res.writeHead(404);
    res.end("Page non trouvée");
  }
});

server.listen(port, () => {
   console.log(`Le serveur est en ligne sur http://${getServerIP()}:${port}`);
});
