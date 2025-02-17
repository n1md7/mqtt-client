import Storage from './storage';

export default class Counter {
  /**
   * @type {Storage}
   */
  #storage;

  constructor(name, start = 0) {
    this.#storage = new Storage('counter', name, start);
  }

  setValue(value = 0) {
    this.#storage.setValue(value);
  }

  getValue() {
    return this.#storage.getValue(0);
  }

  increment() {
    return this.#storage.setValue(+this.#storage.getValue(0) + 1);
  }

  decrement() {
    return this.#storage.setValue(+this.#storage.getValue(0) - 1);
  }

  isZero() {
    return this.#storage.getValue(0) === 0;
  }

  isFinished() {
    return this.#storage.getValue(0) <= 0;
  }
}
