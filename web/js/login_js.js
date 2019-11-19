$("#login").click(function(){
    var walletName=document.getElementById("walletname");
    var txt = walletName.value.trim();
    var privateKey=document.getElementById("privatekey");
    var key=privateKey.value.trim();
    var jsonData={
        walletName: txt,
        walletKey: key
    };
    if(txt.length==0){
        alert("请输入信息！");
    }
    else{
        $.ajax({
            url:"http://127.0.0.1:3000/api/login",
            type:'POST',
            data:jsonData,
            dataType: "json",
            success:function(data){
                if(data.status == 200){
                    alert("登录成功");
                    window.location.href='encrypt.html';
                }
            },
            error:function(XMLHttpRequest, textStatus, errorThrown) {
                if(XMLHttpRequest.status==500){
                    alert("请输入正确的钱包名或私钥！");
                    walletName.value="";
                    walletName.focus();
                }

            }});
    }
});
