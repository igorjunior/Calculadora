var OPERATIONS = {
  SUM : 0, 
  SUB : 1, 
  NON : 2, 
};

function buttonClick(element){
  const content = element.innerHTML;
  if(!isNaN(content)){
    NumberClick(parseFloat(content));
  }else{
    if(content == "+"){
      OperationClick(OPERATIONS.SUM);
    }else if(content == "-"){
      OperationClick(OPERATIONS.SUB)
    }else{
      OperationClick(OPERATIONS.NON)
    }
  }
}