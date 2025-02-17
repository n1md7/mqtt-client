import { clearInterval, Logger, setInterval } from './utils';

/**
 * @typedef {function} Callback
 * @param {number} timestamp
 * @param {Logger} logger
 */

/**
 * @desc Ticker options
 *
 * @typedef {Object} Options
 * @property {Callback} onStart - Start callback, once on timer start
 * @property {Callback} onStop - Stop callback, once on timer stop
 * @property {Callback} onTick - Tick callback, invoking every second
 * @property {Storage} isRunning - Persistent storage whether device was running before force restart/shutdown or not
 * @property {Counter} remainingTime - Remaining timer from persistent storage
 * @property {number} startTime - Start time in seconds
 * @property {number} [interval = 1000] - Tick interval in milliseconds
 */

export default class Ticker {
  /**
   * @readonly
   * @type {Logger}
   */
  #logger;

  /**
   * @type {Timer}
   */
  #timer;

  /**
   * @type {Counter}
   */
  #time;

  /**
   * @type {Storage}
   */
  #isRunning;

  /**
   * @readonly
   * @type {Options}
   */
  #options;

  /**
   * @param {Options} options
   */
  constructor(options) {
    this.#options = options;
    this.#isRunning = options.isRunning;
    this.#time = options.remainingTime;
    this.#logger = new Logger('Timer: ');
  }

  start(seconds = 15) {
    this.#unsubscribe();
    this.#isRunning.setValue(true);
    this.#time.setValue(seconds);
    this.#options.onStart(this.#time.getValue(), this.#logger);
    this.#timer = setInterval(() => {
      this.#options.onTick(this.#time.decrement(), this.#logger);

      if (this.#time.isFinished()) {
        this.stop();
      }
    }, 1000);
  }

  getCurrentTime() {
    return this.#time.getValue();
  }

  getStatus() {
    return this.#isRunning.getValue(false);
  }

  stop() {
    if (!this.#isRunning.getValue()) return;

    this.#logger.log('stopping...');
    this.#isRunning.setValue(false);
    this.#time.setValue(this.#options.startTime);
    this.#options.onStop(this.#time.getValue(), this.#logger);

    this.#unsubscribe();
    this.#logger.log('stopped');
  }

  #unsubscribe() {
    if (this.#timer) {
      clearInterval(this.#timer);
      this.#timer = null;
    }
  }
}
