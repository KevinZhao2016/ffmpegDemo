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

$("#encrypt").click(function (e){
    var path=document.getElementById("path").innerHTML.trim();
    jsonData = {
        filename: path
    }
    $.ajax({
        url:"http://127.0.0.1:3000/api/encrypt",
        type:'POST',
        data:jsonData,
        dataType: "json", 
        success:function(data){
            if(data.status == 200){
                let strongkey = data["strongkey"]
                let weakkey = data["weakkey"]
                let publickey = data["publickey"]
                let privatekey = data["privatekey"]
                let iv = data["iv"]
                document.getElementById("strongkey").innerHTML = strongkey;
                document.getElementById("weakkey").innerHTML = weakkey;
                document.getElementById("publickey").value = publickey;
                document.getElementById("privatekey").value = privatekey;
                document.getElementById("iv").innerHTML = iv;
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