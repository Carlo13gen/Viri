var check = function() {
  if (document.getElementById('password').value ==
    document.getElementById('confirm_password').value) {
    document.getElementById('password').style.borderColor = 'green';
    document.getElementById('password').style.borderWidth = 'thin';
    document.getElementById('confirm_password').style.borderColor = 'green';
    document.getElementById('confirm_password').style.borderWidth = 'thin';
    document.getElementById('button').disabled = false
  } else {
    document.getElementById('password').style.borderColor = 'red';
    document.getElementById('password').style.borderWidth = 'thin';
     document.getElementById('confirm_password').style.borderColor = 'red';
    document.getElementById('confirm_password').style.borderWidth = 'thin';
    document.getElementById('button').disabled = true
  }
}

function character_control(input){
    if (isNullOrWhiteSpace(input.value)) {
        input.value = ""
    }
}

function isNullOrWhiteSpace(input) {

    if (typeof input ==='undefined' || input == null) return true;
    return input.replace(/\s/g, '').length < 1;
}