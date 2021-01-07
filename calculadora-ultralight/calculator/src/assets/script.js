const items = [
  "C",
  "CE",  
  "x^",
  "/",
  "1",
  "2",
  "3",
  "x",
  "4",
  "5",
  "6",
  "-",
  "7",
  "8",
  "9",
  "+",
  ".",
  "0",
  "+/-",
  "=",
];
var OPERATIONS = {
  SUM : 0, 
  SUB : 1, 
  MULT: 2, 
  DIV: 3,
  INVERT: 4,
  POWER: 5,
  CLEAR: 6,
  NON: 7
};
const log = document.getElementById('log');
const textBox = document.getElementById('textBox');
const currentOperationSpan = document.getElementById('currentOperationSpan');
textBox.innerHTML = '0';
let currentResult = false;
let currentOperation = OPERATIONS.SUM;
const buttonClick = (element) => {
  if(currentResult){
    textBox.innerHTML = 0;
    currentResult = false;
  }
  if(isNaN(element)){
    if(element === "."){
      if(!textBox.innerHTML.includes('.')){
        textBox.innerHTML = `${textBox.innerHTML}.`
      }
    }else if(element === "+"){
      currentOperationSpan.innerHTML = "+";
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.SUM;
      currentResult = true;
    }
    else if(element === "-"){
      currentOperationSpan.innerHTML = "-";
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.SUB;
      currentResult = true;
    }
    else if(element === "x"){
      currentOperationSpan.innerHTML = "x";
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.MULT;
      currentResult = true;
    }
    else if(element === "/"){
      currentOperationSpan.innerHTML = "/";
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.DIV;
      currentResult = true;
    }else if(element === "+/-"){
      textBox.innerHTML = Operation(OPERATIONS.INVERT, parseFloat(textBox.innerHTML));
      currentResult = true;
    }else if(element === "="){
      currentOperationSpan.innerHTML = "=";
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.NON;
      currentResult = true;
    }else if(element === "x^"){
      currentOperationSpan.innerHTML = "^";
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.POWER;
      currentResult = true;
    }
    else if(element === "C"){
      textBox.innerHTML = textBox.innerHTML.slice(0, -1);
    }else if(element === "CE"){
      textBox.innerHTML = 0;
      currentOperationSpan.innerHTML = "";
      currentOperation = OPERATIONS.SUM;
      Operation(OPERATIONS.CLEAR, 0);
    }
  }else{
    if(parseFloat(textBox.innerHTML) === 0 && textBox.innerHTML.slice(-1) !== "."){
      textBox.innerHTML = element;
    }else{
      textBox.innerHTML = `${textBox.innerHTML}${element}`;
    }
  }
};

const state = () => {
  return {
  };
};

var map = {};

document.addEventListener('keyup', function(event){
  map[event.keyCode] = event.type == 'keydown';
});
document.addEventListener('keydown', function(event){
  map[event.keyCode] = event.type == 'keydown';
  if(map[16] && map[187]){
    buttonClick("+");
  }
  else if(map[16] && map[56]){
    buttonClick("x");
  }
  else{
  switch (event.keyCode) {
    case 188:
      buttonClick(".")
      break;
      case 190:
        buttonClick(".");
      break;
    case 8:
      buttonClick("C"); 
      break;
      case 46:
        buttonClick("CE");
      break;
      case 13:
        buttonClick("=");
      break;
      case 49:
        buttonClick("1");
        break;
        case 50:
        buttonClick("2");
      break;
      case 51:
        buttonClick("3");
      break;
      case 52:
        buttonClick("4");
      break;
      case 53:
        buttonClick("5");
      break;
      case 54:
        buttonClick("6");
      break;
      case 55:
        buttonClick("7");
      break;
      case 56:
        buttonClick("8");
      break;
      case 57:
        buttonClick("9");
      break;
      case 48:
        buttonClick("0");
      break;
      case 187:
        buttonClick("=");
      break;
      case 189:
        buttonClick("-");
      break;
      case 96:
        buttonClick("0");
      break;
      case 97:
        buttonClick("1");
      break;
      case 98:
        buttonClick("2");
      break;
      case 99:
        buttonClick("3");
      break;
      case 100:
        buttonClick("4");
      break;
      case 101:
        buttonClick("5");
      break;
      case 102:
        buttonClick("6");
      break;
      case 103:
        buttonClick("7");
      break;
      case 104:
        buttonClick("8");
      break;
      case 105:
        buttonClick("9");
      break;
      case 107:
        buttonClick("+");
      break;
      case 109:
        buttonClick("-");
      break;
      case 110:
        buttonClick(".");
      break;
      case 106:
        buttonClick("x");
      break;
      case 111:
        buttonClick("/");
      break;
      case 81:
        buttonClick("/");
        case 222:
        buttonClick("x^");
      break;
    default:
      break;
  }
}
} );

function addLog(number1, number2, operation, result){
  let symbol = "";
  switch (operation) {
    case OPERATIONS.SUM:
      symbol = "+";
      break;
      case OPERATIONS.DIV:
        symbol = "/";
        break;
        case OPERATIONS.MULT:
      symbol = "x";
      break;
      case OPERATIONS.SUB:
      symbol = "-";
      break;
      case OPERATIONS.POWER:
        symbol = "^";
        break;
    default:
      break;
  }
  let logDiv = document.createElement("div");
  logDiv.innerHTML = `<div>${number1} ${symbol} ${number2} = ${result}</div>`;
  logDiv.addEventListener('click', function(e){
    SetResult(result);
    currentOperationSpan.innerHTML = "=";
    textBox.innerHTML = result;
    currentOperation = OPERATIONS.NON;
    currentResult = true;
  });
  log.appendChild(logDiv);
}

function clearLog(){
  log.innerHTML = "";
}


document.getElementById("flushLog").addEventListener('click', function(e) {
  FlushLog();
});