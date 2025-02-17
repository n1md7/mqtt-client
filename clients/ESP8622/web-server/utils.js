import Timer from "timer";

export const setInterval = (callback, delay) => Timer.repeat(callback, delay);
export const clearInterval = (timer) => Timer.clear(timer);

export class Logger {
  #label = null;

  constructor(label = "") {
    this.#label = label;
  }

  log(...message) {
    trace(this.#label + message.join(", "), "\n");
  }
}

export const console = new Logger();

export const HIGH = 1;
export const LOW = 0;
