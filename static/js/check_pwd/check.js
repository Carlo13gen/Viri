var check = function() {
  if (document.getElementById('password').value ==
    document.getElementById('confirm_password').value) {
    document.getElementById('password').style.borderColor = 'green';
    document.getElementById('password').style.borderWidth = 'thin';
    document.getElementById('confirm_password').style.borderColor = 'green';
    document.getElementById('confirm_password').style.borderWidth = 'thin';
  } else {
    document.getElementById('password').style.borderColor = 'red';
    document.getElementById('password').style.borderWidth = 'thin';
     document.getElementById('confirm_password').style.borderColor = 'red';
    document.getElementById('confirm_password').style.borderWidth = 'thin';
  }
}