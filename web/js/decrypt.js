$("#select").click(function (e){//点击导入按钮，使files触发点击事件，然后完成读取文件的操作。
    $("#input").click();
});

var input = document.getElementById("input");
input.addEventListener('change',readFile,false);

function readFile(){
    let file = this.files[0];
    let path = getRootPath() + "video/" + file.name
    document.getElementById("path").innerHTML = file.name
}

function getRootPath(){
    var strFullPath=window.document.location.href;
    var subpath = strFullPath.split('/');
    var rootPath = "";
    for(i = 0; i < subpath.length - 1;i++){
        rootPath += subpath[i] + '/'
    }
    return(rootPath);
}


$("#decrypt").click(function (e){
    let path = document.getElementById("path").innerHTML;
    let weakkey = document.getElementById("strongkey").value;
    let strongkey = document.getElementById("weakkey").value;
    let iv = document.getElementById("iv").value;
    jsonData = {
        filename: path,
        strongkey: strongkey,
        weakkey: weakkey,
        iv: iv
    }
    $.ajax({
        url:"http://127.0.0.1:3000/api/decrypt",
        type:'POST',
        data:jsonData,
        dataType: "json",
        success:function(data){
            if(data.status == 200){
                show(1)
            }
        },
        error:function(XMLHttpRequest, textStatus, errorThrown) {
            if(XMLHttpRequest.status==500){
               show(0)
            }
        }
    });
});

$("#online").click(function (e){
    var path=document.getElementById("path").innerHTML.trim();
    var id=document.getElementById("vid").value;
    jsonData = {
        id: id
    }
    
    $.ajax({
        url:"http://127.0.0.1:3000/api/grab-online",
        type:'POST',
        data:jsonData,
        dataType: "json", 
        success:function(data){
            if(data.status == 200){
                let sign = data.signature
                let publickey=document.getElementById("publickey").value;
                Data = {
                    filename: path,
                    signature: sign,
                    publickey: publickey.replace(/\n/g, '\\\n')
                }
                $.ajax({
                    url:"http://127.0.0.1:3000/api/certificate",
                    type:'POST',
                    data:Data,
                    dataType: "json", 
                    success:function(data){
                        if(data.status == 200){
                            show(3)
                        }else{
                            show(2)
                        }
                    },
                    error:function(XMLHttpRequest, textStatus, errorThrown) {
                        if(XMLHttpRequest.status==500){
                            show(2);
                        }
            
                    }});
            }
        },
        error:function(XMLHttpRequest, textStatus, errorThrown) {
            if(XMLHttpRequest.status==500){
               show(0)
            }
        }
    });
});

$("#offline").click(function (e){
    var path=document.getElementById("path").innerHTML.trim();
    if (path==""){
        alert("请选择视频！");
    }else{
        jsonData = {
            filename: path
        }
        $.ajax({
            url:"http://127.0.0.1:3000/api/grab-offline",
            type:'POST',
            data:jsonData,
            dataType: "json", 
            success:function(data){
                if(data.status == 200){
                    let sign = data.siganture
                    let publickey=document.getElementById("publickey").value;
                    Data = {
                        filename: path,
                        signature: sign,
                        publickey: publickey.replace(/\n/g, '\\\n')
                    }
                    $.ajax({
                        url:"http://127.0.0.1:3000/api/certificate",
                        type:'POST',
                        data:Data,
                        dataType: "json", 
                        success:function(data){
                            if(data.status == 200){
                                show(3)
                            }else{
                                show(2)//todo
                            }
                        },
                        error:function(XMLHttpRequest, textStatus, errorThrown) {
                            if(XMLHttpRequest.status==500){
                               show(2);
                            }
                
                        }});
                }
            },
            error:function(XMLHttpRequest, textStatus, errorThrown) {
                if(XMLHttpRequest.status==500){
                   show(1);
                }
    
            }});
    } 
});