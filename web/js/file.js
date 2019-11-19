$("#select").click(function (e){//点击导入按钮，使files触发点击事件，然后完成读取文件的操作。
	$("#input").click();
});

var input = document.getElementById("input");
input.addEventListener('change',readFile,false);


function readFile(){
	var file = this.files[0];
	alert(file.name)
	var player = videojs('player')
	player.src("video/" + file.name)
	player.load()
	alert(document.getElementById("playsource").src)
}