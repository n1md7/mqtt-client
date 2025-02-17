const [on, off, status, time, errorText, previewTime, selectTime, minutes, seconds] = [
  'on',
  'off',
  'status',
  'time',
  'error-text',
  'preview-time',
  'select-time',
  'minutes',
  'seconds',
].map((id) => document.getElementById(id));

let id;
let countdown = 0;

const getStatusText = (s) => (s ? 'ON' : 'OFF');
const getFormattedTime = (value) => new Date(value * 1000).toISOString().slice(11, 19);
const handleError = (e) => (errorText.innerText = e.message);
const addQueryString = (url) => {
  if (!url.startsWith('/api/on')) return url;

  return `${url}?min=${minutes.value}&sec=${seconds.value}`;
};
const showSelect = () => {
  previewTime.style.display = 'none';
  selectTime.style.display = 'block';
};
const hideSelect = () => {
  previewTime.style.display = 'block';
  selectTime.style.display = 'none';
};
const startCountdown = () => {
  id = setInterval(() => {
    time.innerText = getFormattedTime(--countdown);
    if (countdown === 0) {
      clearInterval(id);
      status.innerText = getStatusText(false);
      status.classList.remove('text-success');
      time.innerText = '00:00';
      id = null;
      showSelect();
    }
  }, 1000);
};
const handleClick = (e) => {
  return fetch(addQueryString(e.target.dataset.path))
    .then((res) => {
      errorText.innerText = '';

      if (res.status === 400) return res.json().then((r) => Promise.reject(r));
      if (res.ok) return res.json();

      return Promise.reject(res.statusText);
    })
    .then((timer) => {
      status.innerText = getStatusText(timer.active);
      status.classList.remove('text-success');
      time.innerText = '00:00';
      hideSelect();

      if (id) clearInterval(id);
      if (timer.active) {
        status.classList.add('text-success');
        countdown = timer.time;
        time.innerText = getFormattedTime(countdown);
        startCountdown();
      } else {
        showSelect();
      }
    })
    .catch(handleError);
};

fetch('/api/status')
  .then((res) => (res.ok ? res.json() : Promise.reject(res.statusText)))
  .then((timer) => {
    status.innerText = getStatusText(timer.active);
    time.innerText = getFormattedTime(timer.time);
    if (timer.active) {
      status.classList.add('text-success');
      countdown = timer.time;
      startCountdown();
      hideSelect();
    } else showSelect();
  })
  .catch(handleError);

on.onclick = handleClick;
off.onclick = handleClick;
