

/*---------------------------
 Defaults for Reveal
----------------------------*/

/*---------------------------
 Listener for data-reveal-id attributes
----------------------------*/

function show(e) {
    if(e==0){
        $('#myModal_fail').reveal();
    }else if(e==1) {
        $('#myModal_suce').reveal();
    }else if(e==2){
        $('#myModal_fail_verify').reveal();
    }else if(e==3){
        $('#myModal_suce_verify').reveal();
    }

}

/*---------------------------
 Extend and Execute
----------------------------*/

$.fn.reveal = function() {
    var defaults = {
        animation: 'fadeAndPop', //fade, fadeAndPop, none
        animationspeed: 300, //how fast animtions are
        closeonbackgroundclick: true, //if you click background will modal close?
        dismissmodalclass: 'close-reveal-modal' //the class of a button or element that will close an open modal
    };

    //Extend dem' options
    var options = $.extend({}, defaults);

    return this.each(function() {

        /*---------------------------
         Global Variables
        ----------------------------*/
        var modal = $(this),
            topMeasure  = parseInt(modal.css('top')),
            topOffset = modal.height() + topMeasure,
            locked = false,
            modalBG = $('.reveal-modal-bg');

        /*---------------------------
         Create Modal BG
        ----------------------------*/
        if(modalBG.length == 0) {
            modalBG = $('<div class="reveal-modal-bg" />').insertAfter(modal);
        }

        /*---------------------------
         Open & Close Animations
        ----------------------------*/
        //Entrance Animations
        modal.bind('reveal:open', function () {
            modalBG.unbind('click.modalEvent');
            $('.' + options.dismissmodalclass).unbind('click.modalEvent');
            if(!locked) {
                lockModal();
                modal.css({'top': $(document).scrollTop()-topOffset, 'opacity' : 0, 'visibility' : 'visible'});
                modalBG.fadeIn(options.animationspeed/2);
                modal.delay(options.animationspeed/2).animate({
                    "top": $(document).scrollTop()+topMeasure + 'px',
                    "opacity" : 1
                }, options.animationspeed,unlockModal());
            }
            modal.unbind('reveal:open');
        });

        //Closing Animation
        modal.bind('reveal:close', function () {
            if(!locked) {
                lockModal();
                modalBG.delay(options.animationspeed).fadeOut(options.animationspeed);
                modal.animate({
                    "top":  $(document).scrollTop()-topOffset + 'px',
                    "opacity" : 0
                }, options.animationspeed/2, function() {
                    modal.css({'top':topMeasure, 'opacity' : 1, 'visibility' : 'hidden'});
                    unlockModal();
                });
            }
            modal.unbind('reveal:close');
        });

        /*---------------------------
         Open and add Closing Listeners
        ----------------------------*/
        //Open Modal Immediately
        modal.trigger('reveal:open')

        //Close Modal Listeners
        var closeButton = $('.' + options.dismissmodalclass).bind('click.modalEvent', function () {
            modal.trigger('reveal:close')
        });

        if(options.closeonbackgroundclick) {
            modalBG.css({"cursor":"pointer"})
            modalBG.bind('click.modalEvent', function () {
                modal.trigger('reveal:close')
            });
        }
        $('body').keyup(function(e) {
            if(e.which===27){ modal.trigger('reveal:close'); } // 27 is the keycode for the Escape key
        });


        /*---------------------------
         Animations Locks
        ----------------------------*/
        function unlockModal() {
            locked = false;
        }
        function lockModal() {
            locked = true;
        }

    });//each call
}//orbit plugin call

function changeM() {
    var inputJ = $("#changeMore"),
        input = inputJ[0],
        Con = $("#ImgCon");

    inputJ.change(function (e) {
        var file = e.target.files[0],//�õ�ԭʼ����
            thisType = file.type,//��ȡ����������ƣ����ж��ļ�����
            thisSize = file.size,//�ļ��Ĵ�С
            reader = new FileReader();

        //readAsDataURL(file),��ȡ�ļ������ļ�������URL����ʽ������result��������
        reader.readAsDataURL(file);

        //�ļ����سɹ��Ժ���Ⱦ��ҳ��
        reader.onload = function (e) {
            var $img = $('<img>').attr("src", e.target.result);
            Con.append($img)
        }
    });

}//changeM end!
changeM();
        
