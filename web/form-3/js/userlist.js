$("#main").ready(function () {
    $.ajax({
        url:"http://127.0.0.1:3000/api/query",
        type:"POST",
        dataType:"json",
        success:function (data) {
        	res = JSON.parse(data.data);
        	for (let i of res) {
        		htmlStr = '<div class="col-sm-3 layout-box">';
        		htmlStr += '<a>';
                base = decodeURIComponent(i.data);
        		htmlStr +=  '<img src="' + base + '" alt="">';
        		htmlStr += '</a>';
        		htmlStr += '<p>'+ i.owner +'<br>'+ "ID：" + i.id +'</p>';
        		$("#main").after(htmlStr);
        	}
        },
        error:function () {
        }
    });
   
});
