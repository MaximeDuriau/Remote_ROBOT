document.addEventListener("DOMContentLoaded", function () {
  let directionPressed = null;
  let motorSpeed = 1;
  let distance = 1; // Vitesse initiale
  var needle = document.getElementById("needle");

  // Appel des fonctions pour afficher le message du servo moteur et la vitesse du moteur dès que la page est chargée
  displayServoMessage(1); // Par exemple, afficher le message du servo moteur numéro 1
  displayMotorSpeed(1); // Par exemple, afficher la vitesse initiale du moteur

  // Récupérer l'élément de la batterie
  const batteryElement = document.getElementById("battery");

  // Récupérer l'élément du pourcentage de batterie
  const batteryPercentageElement =
    document.getElementById("batteryPercentage");

  // Récupérer l'élément de remplissage de la batterie
  const batteryFillElement = document.getElementById("batteryFill");

  // Récupérer le bouton "Cliquez ici"
  const buttonN = document.querySelector(".my-buttonN");
  const buttonA = document.querySelector(".my-buttonA");

  const socket = new WebSocket('ws://192.168.1.33:5000');
  
  socket.addEventListener('open', function (event) {
    console.log('Connexion WebSocket établie.');
    // Envoyer un message pour identifier ce client comme un navigateur
    socket.send('BROWSER');
  });
  
  // Événement lorsque le serveur envoie un message
  socket.addEventListener('message', function (event) {
    const message = event.data;
    console.log('Message reçu du serveur:', message);
  
    if (message.includes('cm/s')) {
      updateSpeed(message);
  } else if (message.includes('cm')) {
      Distance(message);
  }
  });

  function updateSpeed(speed) {
    // Limiter la vitesse à 100 cm/s (pour un affichage visuel)
    if (speed > 100) {
      speed = 100;
    }
    const messageContainer = document.getElementById("compteurVitesse");
    messageContainer.innerText =
      "Vitesse : " + speed;
    // Calculer l'angle de rotation de l'aiguille
    var angle = speed * 1.8 - 90; // Conversion de cm/s en degrés (1 cm/s = 1.8 degrés)
    // Appliquer la rotation à l'aiguille
    needle.style.transform = "rotate(" + angle + "deg)";
  }
 
  updateSpeed(20);
  Distance(2);

  // Ajouter un gestionnaire d'événements de clic au bouton
  buttonN.addEventListener("click", function () {
    // Envoyer un message au serveur
    sendDirectionToServer("PosNeutre");
  });

  buttonA.addEventListener("click", function () {
    // Envoyer un message au serveur
    sendDirectionToServer("PosAt");
  });

  // Fonction pour mettre à jour le pourcentage de batterie et ajuster la largeur de l'élément de remplissage
  function updateBatteryPercentage(percentage) {
    // Mettre à jour le texte du pourcentage de batterie
    batteryPercentageElement.textContent = percentage + "%";
    // Ajuster la largeur de l'élément de remplissage en fonction du pourcentage
    batteryFillElement.style.transform = `scaleX(${percentage / 100})`;
  }
  // Exemple d'utilisation de la fonction pour définir le pourcentage de batterie à 75%
  updateBatteryPercentage(100);

  // Fonction pour envoyer la direction au serveur
  function sendDirectionToServer(direction) {
    fetch("/direction", {
      method: "POST",
      body: direction, // Envoyer la direction sous forme de données brutes
    })
      .then(function (response) {
        if (!response.ok) {
          throw new Error(
            "Erreur lors de la requête HTTP : " + response.status
          );
        }
        console.log("Direction envoyée avec succès:", direction);
      })
      .catch(function (error) {
        console.error(
          "Erreur lors de l'envoi de la direction :",
          error.message
        );
      });
  }

  // Fonction pour afficher le message du servo moteur sur la page HTML
  function displayServoMessage(servoNumber) {
    const messageContainer = document.getElementById("servoMessage");
    messageContainer.innerText =
      "Vous contrôlez le servo moteur numéro " + servoNumber;
  }

  // Fonction pour afficher la vitesse du moteur sur la page HTML
  function displayMotorSpeed(speed) {
    const speedContainer = document.getElementById("motorSpeed");
    speedContainer.innerText = "Vitesse du moteur : " + speed+ "/3";
  }

  function Distance (distance){
    const messageContainer = document.getElementById("compteurDistance");
    messageContainer.innerText =
      "Distance de l'objet : " + distance;
  }

  // Écoute des événements clavier
  document.addEventListener("keydown", function (event) {
    if (!directionPressed) {
      switch (event.key) {
        case "ArrowUp":
          directionPressed = "UP";
          break;
        case "ArrowDown":
          directionPressed = "DOWN";
          break;
        case "ArrowLeft":
          directionPressed = "LEFT";
          break;
        case "ArrowRight":
          directionPressed = "RIGHT";
          break;
        case "+":
          directionPressed = "INCREASE";
          break;
        case "-":
          directionPressed = "DECREASE";
          break;
        case "1":
          directionPressed = "1";
          displayServoMessage(1);
          break;
        case "2":
          directionPressed = "2";
          displayServoMessage(2);
          break;
        case "3":
          directionPressed = "3";
          displayServoMessage(3);
          break;
        case "4":
          directionPressed = "4";
          displayServoMessage(4);
          break;
        case "5":
          directionPressed = "5";
          displayServoMessage(5);
          break;
        case "6":
          directionPressed = "6";
          displayServoMessage(6);
          break;
        case "Shift":
          // Augmenter la vitesse de 1, réinitialiser à 1 si on atteint 4
          directionPressed = "SHIFT";
          motorSpeed = motorSpeed < 3 ? motorSpeed + 1 : 1;
          displayMotorSpeed(motorSpeed);
          break;
        default:
          // Ne rien faire si la touche n'est pas une flèche directionnelle, + ou -
          return;
      }
      sendDirectionToServer(directionPressed);
    }
  });

  document.addEventListener("keyup", function (event) {
    switch (event.key) {
      case "ArrowUp":
      case "ArrowDown":
      case "ArrowLeft":
      case "ArrowRight":
        sendDirectionToServer("CLEAR");
        directionPressed = null;
        break;
      case "+":
      case "-":
        sendDirectionToServer("CLEARPOS");
        directionPressed = null;
        break;
      default:
        directionPressed = null;
        break;
    }
  });
});
