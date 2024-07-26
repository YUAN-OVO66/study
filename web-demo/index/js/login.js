document.getElementById('loginForm').addEventListener('submit', function(event) {
    event.preventDefault(); // 阻止表单的默认提交行为

    const username = document.getElementById('username').value;
    const password = document.getElementById('password').value;

    axios.post('http://127.0.0.1:8080/login', {
        username: username,
        password: password
    })
    .then(function(response) {
        
    }).catch(function(error) {
        

    });
    
});