import Preference from 'preference';

export default class Storage {
  #domain = 'no-domain';
  #name = 'no-name';

  /**
   * @description Persistent on-disk storage
   * @param {string} domain - Like a namespace
   * @param {string} name - Key of the record
   * @param {*} defaultValue
   */
  constructor(domain, name, defaultValue = null) {
    this.#domain = domain;
    this.#name = name;

    if (defaultValue) this.setValue(defaultValue);
  }

  getValue(defaultValue = null) {
    return Preference.get(this.#domain, this.#name) || defaultValue;
  }

  setValue(value) {
    Preference.set(this.#domain, this.#name, value);

    return value;
  }
}
