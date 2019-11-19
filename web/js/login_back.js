
jQuery(document).ready(function() {

    /*
        Fullscreen background
    */
    $.backstretch("form-1/assets/img/backgrounds/1.jpg");

    /*
        Form validation
    */
    $('.login-form input[type="text"], .login-form input[type="number"], .login-form textarea').on('focus', function() {
        $(this).removeClass('input-error');
    });

    $('#login').on('click', function(e) {

        $('.login-form').find('input[type="text"], input[type="number"], textarea').each(function(){
            if( $(this).val() == "" ) {
                e.preventDefault();
                $(this).addClass('input-error');
            }
            else {
                $(this).removeClass('input-error');
            }
        });

    });


});
