
/**************************************
****    TrenoLEGO - Toast popup    ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Show the gamepad settings ****/

function GamepadHelp() 
{
  document.querySelector('.controller').classList.toggle('show');
}


/**** Show the message ****/

function ToastMessage(Message) 
{
  const container = document.getElementById('toast-container');
  const toast     = document.createElement('div');
  
  toast.className = 'toast';
  toast.innerText = Message;
  container.appendChild(toast);
            
  setTimeout(() => {
    toast.classList.add('show');
  }, 10);
            
  /**** Fade-out ****/
  
  setTimeout(() => {
    toast.classList.remove('show');
                
    setTimeout(() => {
      container.removeChild(toast);
    }, 500);
  }, 2500);
}
