/**
 * 常用工具脚本
 * User: 郭艺强
 * Date: 2012-11-15
 */
var JSCore = {};
JSCore.Util = {
    isBlank:function (text) {
        return text == undefined || text == null || this.trim(text) == "";

    },

    isNumber:function (text) {
        if (this.isBlank(text)) {
            return false;
        }
        return text.match("^[0-9]+$");
    },

    isAmount:function (text) {
        if (this.isBlank(text)) {
            return false;
        }
        return text.match("^[1-9][0-9]*$");
    },

    isLength:function (min, max, text) {
        if (this.isBlank(text)) {
            return false;
        }
        if (min <= text.length && text.length <= max) {
            return true;
        } else {
            return false;
        }
    },

    isLarge:function (min, max, text) {
        var amount = Number(text);
        if (min <= amount && max >= amount) {
            return true;
        }
        return false;
    },

    /**
     * 功能：将浮点数四舍五入，取小数点后2位，如果不足2位则补0,这个函数返回的是字符串的格式
     * 用法：changeTwoDecimal(3.1415926) 返回 3.14
     * changeTwoDecimal(3.1) 返回 3.10
     * @param {} floatvar
     * @return {Boolean}
     */
    changeTwoDecimal_f:function (floatvar) {
        var f_x = parseFloat(floatvar);
        if (isNaN(f_x)) {
            alert('function:changeTwoDecimal->parameter error');
            return false;
        }
        var f_x = Math.round(f_x * 100) / 100;
        var s_x = f_x.toString();
        var pos_decimal = s_x.indexOf('.');
        if (pos_decimal < 0) {
            pos_decimal = s_x.length;
            s_x += '.';
        }
        while (s_x.length <= pos_decimal + 2) {
            s_x += '0';
        }
        return s_x;
    },

    /**
     * 判断一个值是否在一个数组中
     * @param {} needle
     * @param {} haystack
     * @return {Boolean}
     */
    inArray:function (needle, haystack) {
        type = typeof needle
        if (type == 'string' || type == 'number') {
            for (var i in haystack) {
                if (haystack[i] == needle) {
                    return true;
                }
            }
        }
        return false;
    },
    trim:function (text) {
        var trimLeft = /^\s+/,
            trimRight = /\s+$/;
        return text == null ?
            "" :
            text.toString().replace(trimLeft, "").replace(trimRight, "");
    },
    /**
     * 客户端版本对比，以点号分割
     * @param {} curVer 当前版本号 如2.2.1.1
     * @param {} compareVer 对比的版本号 如2.4.1
     * @return {Number} 0：等于，-1：小于，1：大于
     */
    compareVer : function(curVer, compareVer) {
    	var arr_curVer = curVer.split(".");
    	var arr_compareVer = compareVer.split(".");
    	for ( var i = 0; (i < arr_curVer.length && i < arr_compareVer.length); i++) {
    		var c = Number(arr_curVer[i]);
    		var p = Number(arr_compareVer[i]);
    		if (c > p) {
    			return 1;
    		} else if (c < p) {
    			return -1;
    		}
    	}
    	return 0;
    }
};