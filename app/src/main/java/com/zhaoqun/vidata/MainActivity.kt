package com.zhaoqun.vidata

import android.Manifest
import android.content.pm.PackageManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.view.Surface
import android.view.SurfaceHolder
import android.widget.TextView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.zhaoqun.vidata.databinding.ActivityMainBinding
import java.io.File


class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        checkPermission(Manifest.permission.CAMERA, CAMERA_PERMISSION_CODE)
        checkPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, STORAGE_PERMISSION_CODE)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        var print_string = ""
        // start some dummy thread that is different from UI thread
        Thread(Runnable {
            while(true){
                Thread.sleep(10)
                print_string = stringFromJNI()
                runOnUiThread {
                    binding.displayString.text = print_string
                }
            }
        }).start()


        binding.camPreview.holder.addCallback(
            object : SurfaceHolder.Callback {
                override fun surfaceCreated(holder: SurfaceHolder) {
                    sendSurfaceToJNI(holder.surface)
                }
                override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
                override fun surfaceDestroyed(holder: SurfaceHolder) {}
            }
        )
        binding.camPreview.setOnClickListener {
            takeRgbSingleShotJNI()
        }

        binding.dump.setOnCheckedChangeListener { _, isChecked ->
            if (isChecked) startDumpJNI() else stopDumpJNI()
        }

    }

    override fun onStart() {
        super.onStart()
        copyConfigToSDcard()
    }

    override fun onStop() {
        super.onStop()

    }

    override fun onResume() {
        super.onResume()
        startJNI()
    }
    override fun onPause() {
        super.onPause()
        stopJNI()
    }


    private fun copyConfigToSDcard() {
        val sdcard_path =  "sdcard/VIdata/"
        val file_name = "config.yaml"
        var do_copy : Boolean = false
        if (File(sdcard_path).exists()) {
            if (File(sdcard_path + file_name).exists())
                return
            else
                do_copy = true
        } else {
            File(sdcard_path).mkdirs()
            do_copy = true
        }

        if (do_copy) {
            assets.open(file_name).use { stream ->
                File(sdcard_path + file_name).outputStream().use {
                    stream.copyTo(it)
                }
            }
        }
    }

    private fun checkPermission(permission: String, requestCode: Int) {
        if (ContextCompat.checkSelfPermission(this@MainActivity, permission) == PackageManager.PERMISSION_DENIED) {
//            Requesting the permission
            ActivityCompat.requestPermissions(this@MainActivity, arrayOf(permission), requestCode)
        } else {
//            Toast.makeText(this@MainActivity, permission + " permission already granted", Toast.LENGTH_SHORT).show()
        }
    }
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun startJNI()
    external fun stopJNI()
    //    external fun sendPathToJNI(app_storage: String)
    external fun sendSurfaceToJNI(cam_sf: Surface)
    external fun stringFromJNI() : String
    external fun startDumpJNI()
    external fun stopDumpJNI()
    external fun takeRgbSingleShotJNI()

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("vidata")
        }
        private const val CAMERA_PERMISSION_CODE = 100
        private const val STORAGE_PERMISSION_CODE = 200
    }
}