/*
 * ========================= Camera.cpp ==========================
 *                          -- tpr --
 *                                        创建 -- 2018.11.24
 *                                        修改 -- 2018.11.24
 * ----------------------------------------------------------
 *   摄像机 类
 * ----------------------------
 */

#include "Camera.h"

//-------------------- CPP --------------------//
#include <cassert> //- assert
#include <string>
//#include <iostream>

//-------------------- Engine --------------------//
#include "config.h" // SCR_WIDTH, SCR_HEIGHT
#include "srcs_engine.h" //- 所有资源


//using std::cout;
//using std::endl;
using std::string;

//---------------------- 局部 变量 ---------------------
namespace{
    Camera *camera_p_current = nullptr; //-- 当前摄像机 指针
}


/* ===========================================================
 *                        init    
 * -----------------------------------------------------------
 * --  真正的初始化函数
 */
void Camera::init(){

    //update_camera_vectors();
}


/* ===========================================================
 *                     update_mat4_view    
 * -----------------------------------------------------------
 * --  生成 观察矩阵, 并将 观察矩阵 的指针 (const float *) 返回出去
 */
glm::mat4 &Camera::update_mat4_view(){

    mat4_view = glm::lookAt( cameraPos, 
                             (cameraPos + cameraFront), 
                             cameraUp );

    return mat4_view;
}


/* ===========================================================
 *                update_mat4_projection  
 * -----------------------------------------------------------
 * --  生成 ortho 投影矩阵, 并将 投影矩阵 的指针 (const float *) 返回出去
 * --  
 */
glm::mat4 &Camera::update_mat4_projection(){

    //-- 在未来，WORK_WIDTH／WORK_HEIGHT 会成为变量（随窗口尺寸而改变）
    //   所以不推荐，将 ow/oh 写成定值
    float ow = (float)(WORK_WIDTH/2);  //- 横向边界半径（像素）
    float oh = (float)(WORK_HEIGHT/2); //- 纵向边界半径（像素）

    //------ 近平面 / 远平面 --------
    // 对这组值的理解：
    //  沿着 摄像机的视角，从摄像机当前pos出发，
    //  向“前”推进 zNear 获得的平面就是 近平面
    //  向“前”推进 zFar  获得的平面就是 远平面
    //----------
    //  由于我们的 二维游戏，摄像机始终朝向 负z轴，
    //  所以这两个值就是沿着 fz轴的叠加
    //
    //-- 每一帧都要计算 近平面/远平面，根据 camera pos.y --
    //   z = -y;
    //   camera 在 z轴其实是存在运动的，以此确保 渲染盒子始终匹配图元
    //
    float zNear = 0.1f + cameraPos.y;
    float zFar  = 2000.0f  + cameraPos.y;


    mat4_projection = glm::ortho( -ow,   //-- 左边界
                                   ow,   //-- 右边界
                                  -oh,   //-- 下边界
                                   oh,   //-- 上边界
                                   zNear, //-- 近平面
                                   zFar  //-- 远平面
                                );

    return mat4_projection;
}


/* ===========================================================
 *           cameraPos_up / down / left / right   
 * -----------------------------------------------------------
 * --  通过 wsad 键，控制摄像机 前后左右移动
 */
//void Camera::cameraPos_forward(){
    /*
    cameraSpeed = 2.5f * get_deltaTime();
    cameraPos += cameraFront * cameraSpeed;
    */
//}


//void Camera::cameraPos_back(){
    /*
    cameraSpeed = 2.5f * get_deltaTime();
    cameraPos -= cameraFront * cameraSpeed;
    */
//}


void Camera::cameraPos_left(){
    cameraSpeed = 30.0f * esrc::timer.get_last_deltaTime();
    cameraPos -= cameraRight * cameraSpeed;
}


void Camera::cameraPos_right(){
    cameraSpeed = 30.0f * esrc::timer.get_last_deltaTime();
    cameraPos += cameraRight * cameraSpeed;
}


void Camera::cameraPos_up(){
    cameraSpeed = 30.0f * esrc::timer.get_last_deltaTime();
    cameraPos += cameraUp * cameraSpeed;
}

void Camera::cameraPos_down(){
    cameraSpeed = 30.0f * esrc::timer.get_last_deltaTime();
    cameraPos -= cameraUp * cameraSpeed;
}


/* ===========================================================
 *                    mousePos_move     
 * -----------------------------------------------------------
 * -- 鼠标位移，控制 摄像机 视角。
 */
void Camera::mousePos_move( double xpos, double ypos ){

    //--- 游戏开始时，第一次鼠标运动 时的 配置。
    if( 1 == fst_mouse ){
        mouseX_last = xpos;
        mouseY_last = ypos;
        fst_mouse = 0;
    }

    //---- 更新 鼠标坐标
    mouseX_off = xpos - mouseX_last;
    mouseY_off = -(ypos - mouseY_last); //-- 反向 一下。

    mouseX_last = xpos;
    mouseY_last = ypos;

    //---- 
    mouseX_off *= mousePos_sensitivity;
    mouseY_off *= mousePos_sensitivity;

    yaw   += mouseX_off; //-- 偏航（角度）
    pitch += mouseY_off; //-- 俯仰（角度）

    //------ 避免 俯仰值 到达 90度 （看向天顶）
    if( pitch > 89.0f ){
        pitch = 89.0f;
    }
    if( pitch < -89.0f ){
        pitch = -89.0f;
    }

    //--- 因为 camera 姿态发生改变，需要更新 几个核心向量。
    //update_camera_vectors(); 
}


/* ===========================================================
 *                     mouseFov_reset    
 * -----------------------------------------------------------
 * -- 鼠标滚轮，控制 视角 fov ／ field of view
 */
/*
void Camera::mouseFov_reset( double xoffset, double yoffset ){

    if( fov >= 1.0f && fov <= 45.0f ){
        fov -= yoffset;
    }

    if( fov <= 1.0f ){
        fov = 1.0f;
    }
    
    if( fov >= 45.0f ){
        fov = 45.0f;
    }
}
*/

/* ===========================================================
 *                 update_camera_vectors
 * -----------------------------------------------------------
 * -- 刷新 camera 的几个核心向量。 
 * -- 分别在 camera 初始化，以及 camera 视角转变（通过鼠标位移）时，
 * --  在 二维游戏中，camera方向是固定的，不用每帧都运算 本函数将被废弃
 * 
 */
/*
void Camera::update_camera_vectors(){

    //-- 更新 camera 前视向量
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    cameraFront = glm::normalize(front);

    //- 更新 camera 右手向量
    //  在 二维游戏中是固定的，不用每帧都运算
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp)); 
    //- 更新 camera 头顶向量
    //  在 二维游戏中是固定的，不用每帧都运算
    cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront));   

}
*/


/* ===========================================================
 *                      bind_camera_current   
 * -----------------------------------------------------------
 * --  将 参数 cp 绑定为 当前摄像机
 */
/*
void bind_camera_current( const Camera *cp ){
    camera_p_current = const_cast<Camera*>( cp );
}
*/

/* ===========================================================
 *                       camera_current
 * -----------------------------------------------------------
 * --  获得 指向 当前摄像机 的指针
 */
/*
Camera *camera_current(){

    if( camera_p_current == nullptr ){
        cout << "Camera::camera_current: error. \n"
             << "camera_p_current = nullptr"
             << endl;
        assert(0);
    }

    return camera_p_current;
}
*/














