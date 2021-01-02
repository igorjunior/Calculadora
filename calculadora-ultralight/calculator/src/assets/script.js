const items = [
  "CE",
  "+/-",
  "%",
  "/",
  "1",
  "2",
  "3",
  "*",
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
  "DEL",
  "=",
];
var OPERATIONS = {
  SUM : 0, 
  SUB : 1, 
  MULT: 2, 
  DIV: 3,
  INVERT: 4,
  NON: 5
};
const textBox = document.getElementById('textBox');
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
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.SUM;
      currentResult = true;
    }
    else if(element === "-"){
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.SUB;
      currentResult = true;
    }
    else if(element === "*"){
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.MULT;
      currentResult = true;
    }
    else if(element === "/"){
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.DIV;
      currentResult = true;
    }else if(element === "+/-"){
      textBox.innerHTML = Operation(OPERATIONS.INVERT, parseFloat(textBox.innerHTML));
      currentResult = true;
    }else if(element === "="){
      textBox.innerHTML = Operation(currentOperation, parseFloat(textBox.innerHTML));
      currentOperation = OPERATIONS.NON;
      currentResult = true;
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


