function ValidateSize(file) {
        var FileSize = file.files[0].size / 1024 / 1024; // in MB
        if (FileSize > 2) {
            alert('File size exceeds 2 MB');
            f = document.getElementById("file")
            f.value=""
        }
        else
            console.log(file)
    }