$("#import").click(function(){//������밴ť��ʹfiles��������¼���Ȼ����ɶ�ȡ�ļ��Ĳ�����
    $("#files").click();
});

function import(){
    var selectedFile = document.getElementById("files").files[0];//��ȡ��ȡ��File����
    var name = selectedFile.name;//��ȡѡ���ļ����ļ���
    var size = selectedFile.size;//��ȡѡ���ļ��Ĵ�С
    console.log("�ļ���:"+name+"��С��"+size);

    var reader = new FileReader();//�����Ǻ��ģ�������ȡ��������������ɵġ�
    reader.readAsText(selectedFile);//��ȡ�ļ�������

    reader.onload = function(){
        console.log(this.result);//����ȡ���֮���ص����������Ȼ���ʱ�ļ������ݴ洢����result�С�ֱ�Ӳ������ɡ�
    };
}