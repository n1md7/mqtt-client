import Digital from "pins/digital";
import { Logger, HIGH } from "./utils";

export default class Switch extends Digital {
  #onSignal = 1;
  #offSignal = 0;

  /**
   * @readonly
   * @type {Logger}
   */
  #logger;

  /**
   * @param {Object} options
   * @param {number} options.pin Number of the pin
   * @param {string} [options.name = 'Switch'] Logger label
   * @param {number} [options.signal = 1] Either HIGH(1) or LOW(0) to control with.
   */
  constructor(options) {
    super({
      pin: options.pin,
      mode: Digital.Output,
    });
    this.#onSignal = options.signal ?? HIGH;
    this.#offSignal = this.#onSignal ^ 1;
    this.#logger = new Logger(`${options.name || "Switch"} (${options.pin}): `);
    this.stop();
  }

  start() {
    this.#logger.log(`Writing ON signal: ${this.#onSignal}`);
    this.write(this.#onSignal);
    this.#logger.log(`ON signal written.`);
  }

  stop() {
    this.#logger.log(`Writing OFF signal: ${this.#offSignal}`);
    this.write(this.#offSignal);
    this.#logger.log(`OFF signal written.`);
  }
}
