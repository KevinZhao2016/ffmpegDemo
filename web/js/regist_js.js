$('#register').click(function () {
    var uname=document.getElementById("name");
    var uIDcard=document.getElementById("IDcard");
    var uemail=document.getElementById("email");
    var reg = new RegExp("^[a-z0-9]+([._\\-]*[a-z0-9])*@([a-z0-9]+[-a-z0-9]*[a-z0-9]+.){1,63}[a-z0-9]+$");
    if (uname.value==''||uIDcard.value==''||uemail.value==''){
        alert("请输入信息！");
    }else if (!reg.test(uemail.value)){
        alert("请输入正确的邮箱！");
    }else {
        var jsonData={
            name: uname.value,
            IDcard: uIDcard.value,
            email: uemail.value
        };
        $.ajax({
            url:"http://127.0.0.1:3000/api/register",
            type:"POST",
            data:jsonData,
            dataType:"json",
            success:function (data) {
                // alert(data.data);
                res = JSON.parse(data.data);
                // alert("sss")
                document.getElementById("password").innerHTML=res.walletPassword;
                document.getElementById("privatekey").innerHTML=res.privateKey;
                document.getElementById("publickey").innerHTML=res.publicKey;
                $('#info_save').css({"display":"block"});
            },
            error:function(XMLHttpRequest, textStatus, errorThrown) {
                if(XMLHttpRequest.status==500){
                    alert("钱包名不可用！");
                    wname.value="";
                    wname.focus();
                }
            }});
    }
});