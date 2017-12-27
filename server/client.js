var RED_WAIT = '#ff9999'
var YEL_PAYB = '#ffff00'
var GRN_GETM = '#00ff00'
var PUR_SPWN = '#ff99ff'
var WHT_NOTN = "#ffffff"

var prog_opts = {
    color: '#aaa',
    // This has to be the same size as the maximum width to
    // prevent clipping
    strokeWidth: 4,
    trailWidth: 1,
    easing: 'easeOut',
    duration: 500,
    text: { autoStyleContainer: false } ,
    from: { color: '#39700C', width: 1 } ,
    to: { color: '#82FF1C', width: 4 } ,
    // Set default step function for all animate calls
    step: function(state, circle, attachment) {
        circle.path
        .setAttribute('stroke', state.color);
        circle.path
        .setAttribute('stroke-width', state.width);
        var value = Math.round(circle.value() * 100);
        if(value === 100){
            circle.svg.
            parentNode.style.visibility = "hidden";
        }else if(value === 0){
            circle.setText("New");
        }else{
            circle.setText(value + '%');
        }
    }
};

var bar_opts = {
    strokeWidth: 3,
    color: '#131370',
    trailColor: '#eee',
    trailWidth: 1,
    easing: 'easeOut',
    duration: 300,
    svgStyle: null,
    text: {
        value: '',
        alignToBottom: true
    },
    from: {color: '#FF2A1C'} ,
    to: {color: '#87FF42'} ,
    // Set default step function
    // for all animate calls
    step: function(state, bar){
        bar.path.setAttribute('stroke', state.color);
        var value = Math.round(bar.value() * 100);
        if(value < 20){
            bar.setText("Depression");
        }
        else if(value < 40){
            bar.setText("Danger");
        }
        else if(value < 60){
            bar.setText("Considerable");
        }
        else if(value < 80){
            bar.setText("Moderate");
        }
        else{
            bar.setText("Safe");
        }
        bar.text.style.color = state.color;
    }
}


var socket = io.connect();
var cont_bank = $("#cont_sample_bank").clone();
cont_bank.attr("id", "bank").appendTo("#banks");
//cont_bank.show();
var js_bars = {};
var js_cus = {};

for(i=0; i<20; i++){
    var new_cus = $("#cus_cont_sample").clone();
    new_cus.find(".cus_prog").attr("id", ("cus" + (i+1)));
    new_cus.find(".cus_prog").css('visibility', 'hidden');
    new_cus.find(".cus_text").text("Customer " + (i+1));
    new_cus.find(".cus_text_bot").text("Empty");
    $('#cus_row_' + (Math.floor(i / 5) + 1)).append(new_cus);
    new_cus.show();
}

for(i=0; i<20; i++){
    var bar = new ProgressBar.Circle(('#cus' + (i+1)), prog_opts);
    bar.text.style.fontFamily = '"Raleway", Helvetica, sans-serif';
    bar.text.style.fontSize = '2rem';
    bar.text.style.color = '#131370';
    //bar.animate( (i+1)/20 );  // Number from 0.0 to 1.0
    js_bars['cus' + (i+1) ] = bar;
    js_cus['cus' + (i+1) ] = { "max": -1, "cur": 0 };
}

var bank_bar = new ProgressBar.SemiCircle( '#bank_prog', bar_opts);
bank_bar.text.style.fontFamily = '"Raleway", Helvetica, sans-serif';
bank_bar.text.style.fontSize = '4.5rem';
bank_bar.set(1);
$('#bank_text').text( "100,000/100,000");
$('#bank_text').css('font-family', '"Raleway", Helvetica, sans-serif');


//bars &
socket.on('init', function(data){
    console.log(data.json);
    var js_update = data.json;
    var last_coin = js_update['bank']['cur'];
    for(i = 0; i < 20; i++){
        var new_max = js_update['cus' + (i+1)]['max'];
        var new_cur = js_update['cus' + (i+1)]['cur'];
        var wait    = js_update['cus' + (i+1)]['wait'];
        var bar = js_bars['cus' + (i+1)];
        var prog = $("#cus" + (i+1));
        //if(new_cur > 0){
            //    last_coin -= new_cur;
            //}
            if(wait > 0){
            $('#' + 'cus' + (i + 1)).parent().animate({backgroundColor: RED_WAIT}, 3000);
        }
        if(new_cur === 0 && new_max != -1){
            bar.set(0);
            prog.css('visibility', 'visible');
            prog.siblings(".cus_text_bot").text(new_cur + '/' + new_max);
        }else if(new_cur > 0){
            prog.css('visibility', 'visible');
            prog.siblings(".cus_text_bot").text(new_cur + '/' + new_max);
            bar.animate(new_cur / new_max);
        }else{
            //prog.css('visibility') === 'hidden'
            prog.siblings(".cus_text_bot").text("Empty");
            prog.siblings(".cus_text_bot").css('fontSize', '17px');
        }

        if(new_max > 9999 && new_max !== new_cur){
            prog.siblings(".cus_text_bot").css('fontSize', '13px');
        }
    }
    if(last_coin > 999){
        head_digs = Math.floor(last_coin / 1000);
        if(last_coin % 1000 === 0){
            $('#bank_text').text(head_digs + ",000/100,000");
        }
        else{
            $('#bank_text').text(head_digs + "," + ('0' + Math.floor(last_coin % 1000)).slice(-3) + "/100,000");
        }
    }
    else{
        $('#bank_text').text(last_coin + "/100,000");
    }
    bank_bar.animate(last_coin/100000);
});

socket.on('update', function(data){
    console.log(data);
    for(var i = 0 ; i < data.length ; i++){
        for(var key in data[i]){
            obj = data[i][key];
            if(key.indexOf('-') >= 0){
                key = key.replace('-','');
                obj = $('#' + key);
                obj.parent().animate({backgroundColor: RED_WAIT}, 3000);
            }
            else{
                if(key != 'bank'){
                    $('#' + key).parent().stop();
                    var prog = $("#" + key);
                    if(obj['cur'] === obj['max']){
                        $('#' + key).parent().css('background-color', GRN_GETM);;
                        prog.siblings(".cus_text_bot").text("Empty");
                        prog.siblings(".cus_text_bot").css('fontSize', '17px');
                        prog.css('visibility', 'invisible');
                        js_bars[key].animate(obj['cur'] / obj['max']);
                    }
                    else{
                        if(obj['cur'] > 0){
                            $('#' + key).parent().css('background-color', YEL_PAYB);;
                        }
                        else{
                            $('#' + key).parent().css('background-color', PUR_SPWN);;
                        }
                        prog.siblings(".cus_text_bot").text(obj['cur'] + '/' + obj['max']);
                        js_bars[key].animate(obj['cur'] / obj['max']);
                        prog.siblings(".cus_text_bot").css('fontSize', '13px');
                        prog.css('visibility', 'visible');
                    }
                $('#' + key).parent().animate({backgroundColor: WHT_NOTN}, 'slow');
            }
            else{
                if(obj['cur'] > 999){
                    head_digs = Math.floor(obj['cur'] / 1000);
                    if(obj['cur'] % 1000 === 0){
                        $('#bank_text').text(head_digs + ",000/100,000");
                    }
                    else{
                        $('#bank_text').text(head_digs + "," + ('0' + Math.floor(obj['cur'] % 1000)).slice(-3) + "/100,000");
                    }
                }
                else{
                    $('#bank_text').text(obj['cur'] + "/100,000");
                }
                bank_bar.animate(obj['cur']/100000);
            }
        }
    }
}
});
