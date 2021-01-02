const items = [
  "CE",
  "+/-",
  "%",
  "/",
  "1",
  "2",
  "3",
  "X",
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

const operations = {
  "+": (itemA, itemB) => {
    return itemA + itemB;
  },
  "-": (itemA, itemB) => {
    return itemA - itemB;
  },
  "/": (itemA, itemB) => {
    return itemA / itemB;
  },
  X: (itemA, itemB) => {
    return itemA * itemB;
  },
};
const buttonClick = (element) => {
  const content = element.innerHTML;
  if (isNaN(content)) {
    if (operations.includes(content)) {
      console.log(content);
    }
  }
};

const state = () => {
  return {
    itemOne: "",
    itemTwo: "",
    operation: "",
    reset: false,
    checkValidity: () => {
      if (this.itemOne && this.itemTwo && this.operation !== null) {
        return true;
      }
      return false;
    },
    addNumber(content) {
      if (this.reset) {
        if (!isNaN(content) || content === ".") {
          this.itemOne = Number(`${content}`);
        } else {
          if (!this.operation) {
            if (Object.keys(operations).includes(content)) {
              this.operation = content;
            }
          }
        }
        this.reset = false;
        return 0;
      }
      if (!isNaN(content) || content === ".") {
        if (content === ".") {
          if (this.operation) {
            if (!isNaN((this.itemTwo = Number(`${this.itemTwo}${content}`)))) {
              this.itemTwo = `${this.itemTwo}${content}`;
            }
          } else {
            if (!isNaN((this.itemOne = Number(`${this.itemOne}${content}`)))) {
              this.itemOne = `${this.itemOne}${content}`;
            }
          }
          return 0;
        }
        if (this.operation) {
          this.itemTwo = Number(`${this.itemTwo}${content}`);
        } else {
          this.itemOne = Number(`${this.itemOne}${content}`);
        }
      } else {
        if (!this.operation) {
          if (Object.keys(operations).includes(content)) {
            this.operation = content;
          }
        }
      }
      if (content === "=") {
        this.itemOne = String(
          operations[this.operation](this.itemOne, this.itemTwo)
        );
        this.operation = "";
        this.itemTwo = "";
        this.reset = true;
      } else if (content === "CE") {
        this.itemOne = "";
        this.operation = "";
        this.itemTwo = "";
        this.reset = true;
      } else if (content === "+/-") {
        if (this.operation) {
          this.itemOne = String(
            -1 * operations[this.operation](this.itemOne, this.itemTwo)
          );
          this.operation = "";
          this.itemTwo = "";
        } else {
          this.itemOne = String(-1 * this.itemOne);
          this.operation = "";
          this.itemTwo = "";
        }
      }
    },
  };
};
