/*
   Copyright (C) 2011 Samsung Electronics
   Copyright (C) 2012 Intel Corporation. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

/**
 * @file    ewk_view.h
 * @brief   WebKit main smart object.
 *
 * This object provides view related APIs of WebKit2 to EFL object.
 *
 * The following signals (see evas_object_smart_callback_add()) are emitted:
 *
 * - "intent,request,new", Ewk_Intent_Request*: reports new Web intent request.
 * - "intent,service,register", Ewk_Intent_Service*: reports new Web intent service registration.
 * - "load,error", const Ewk_Web_Error*: reports main frame load failed.
 * - "load,finished", void: reports load finished.
 * - "load,progress", double*: load progress has changed (value from 0.0 to 1.0).
 * - "load,provisional,failed", const Ewk_Web_Error*: view provisional load failed.
 * - "load,provisional,redirect", void: view received redirect for provisional load.
 * - "load,provisional,started", void: view started provisional load.
 * - "policy,decision,navigation", Ewk_Navigation_Policy_Decision*: a navigation policy decision should be taken.
 *   To make a policy decision asynchronously, simply increment the reference count of the
 *   #Ewk_Navigation_Policy_Decision object using ewk_navigation_policy_decision_ref().
 * - "policy,decision,new,window", Ewk_Navigation_Policy_Decision*: a new window policy decision should be taken.
 *   To make a policy decision asynchronously, simply increment the reference count of the
 *   #Ewk_Navigation_Policy_Decision object using ewk_navigation_policy_decision_ref().
 * - "resource,request,failed", const Ewk_Web_Resource_Load_Error*: a resource failed loading.
 * - "resource,request,finished", const Ewk_Web_Resource*: a resource finished loading.
 * - "resource,request,new", const Ewk_Web_Resource_Request*: a resource request was initiated.
 * - "resource,request,response", Ewk_Web_Resource_Load_Response*: a response to a resource request was received.
 * - "resource,request,sent", const Ewk_Web_Resource_Request*: a resource request was sent.
 * - "title,changed", const char*: title of the main frame was changed.
 */

#ifndef ewk_view_h
#define ewk_view_h

#include "ewk_context.h"
#include "ewk_intent.h"
#include "ewk_url_request.h"
#include "ewk_url_response.h"
#include "ewk_web_error.h"
#include "ewk_web_resource.h"
#include <Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_View_Smart_Data Ewk_View_Smart_Data;
typedef struct _Ewk_View_Smart_Class Ewk_View_Smart_Class;

/// Ewk view's class, to be overridden by sub-classes.
struct _Ewk_View_Smart_Class {
    Evas_Smart_Class sc; /**< all but 'data' is free to be changed. */
    unsigned long version;

    // event handling:
    //  - returns true if handled
    //  - if overridden, have to call parent method if desired
    Eina_Bool (*focus_in)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*focus_out)(Ewk_View_Smart_Data *sd);
    Eina_Bool (*mouse_wheel)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Wheel *ev);
    Eina_Bool (*mouse_down)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Down *ev);
    Eina_Bool (*mouse_up)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Up *ev);
    Eina_Bool (*mouse_move)(Ewk_View_Smart_Data *sd, const Evas_Event_Mouse_Move *ev);
    Eina_Bool (*key_down)(Ewk_View_Smart_Data *sd, const Evas_Event_Key_Down *ev);
    Eina_Bool (*key_up)(Ewk_View_Smart_Data *sd, const Evas_Event_Key_Up *ev);
};

/**
 * The version you have to put into the version field
 * in the @a Ewk_View_Smart_Class structure.
 */
#define EWK_VIEW_SMART_CLASS_VERSION 1UL

/**
 * Initializer for whole Ewk_View_Smart_Class structure.
 *
 * @param smart_class_init initializer to use for the "base" field
 * (Evas_Smart_Class).
 *
 * @see EWK_VIEW_SMART_CLASS_INIT_NULL
 * @see EWK_VIEW_SMART_CLASS_INIT_VERSION
 * @see EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION
 */
#define EWK_VIEW_SMART_CLASS_INIT(smart_class_init) {smart_class_init, EWK_VIEW_SMART_CLASS_VERSION, 0, 0, 0, 0, 0, 0, 0, 0}

/**
 * Initializer to zero a whole Ewk_View_Smart_Class structure.
 *
 * @see EWK_VIEW_SMART_CLASS_INIT_VERSION
 * @see EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION
 * @see EWK_VIEW_SMART_CLASS_INIT
 */
#define EWK_VIEW_SMART_CLASS_INIT_NULL EWK_VIEW_SMART_CLASS_INIT(EVAS_SMART_CLASS_INIT_NULL)

/**
 * Initializer to zero a whole Ewk_View_Smart_Class structure and set
 * name and version.
 *
 * Similar to EWK_VIEW_SMART_CLASS_INIT_NULL, but will set version field of
 * Evas_Smart_Class (base field) to latest EVAS_SMART_CLASS_VERSION and name
 * to the specific value.
 *
 * It will keep a reference to name field as a "const char *", that is,
 * name must be available while the structure is used (hint: static or global!)
 * and will not be modified.
 *
 * @see EWK_VIEW_SMART_CLASS_INIT_NULL
 * @see EWK_VIEW_SMART_CLASS_INIT_VERSION
 * @see EWK_VIEW_SMART_CLASS_INIT
 */
#define EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION(name) EWK_VIEW_SMART_CLASS_INIT(EVAS_SMART_CLASS_INIT_NAME_VERSION(name))

typedef struct _Ewk_View_Private_Data Ewk_View_Private_Data;
/**
 * @brief Contains an internal View data.
 *
 * It is to be considered private by users, but may be extended or
 * changed by sub-classes (that's why it's in public header file).
 */
struct _Ewk_View_Smart_Data {
    Evas_Object_Smart_Clipped_Data base;
    const Ewk_View_Smart_Class* api; /**< reference to casted class instance */
    Evas_Object* self; /**< reference to owner object */
    Evas_Object* image; /**< reference to evas_object_image for drawing web contents */
    Ewk_View_Private_Data* priv; /**< should never be accessed, c++ stuff */
    struct {
        Evas_Coord x, y, w, h; /**< last used viewport */
    } view;
    struct { /**< what changed since last smart_calculate */
        Eina_Bool any:1;
        Eina_Bool size:1;
        Eina_Bool position:1;
    } changed;
};

/// Creates a type name for _Ewk_Web_Resource_Request.
typedef struct _Ewk_Web_Resource_Request Ewk_Web_Resource_Request;

/**
 * @brief Structure containing details about a resource request.
 */
struct _Ewk_Web_Resource_Request {
    Ewk_Web_Resource *resource; /**< resource being requested */
    Ewk_Url_Request *request; /**< URL request for the resource */
    Ewk_Url_Response *redirect_response; /**< Possible redirect response for the resource */
};

/// Creates a type name for _Ewk_Web_Resource_Load_Response.
typedef struct _Ewk_Web_Resource_Load_Response Ewk_Web_Resource_Load_Response;

/**
 * @brief Structure containing details about a response to a resource request.
 */
struct _Ewk_Web_Resource_Load_Response {
     Ewk_Web_Resource *resource; /**< resource requested */
     Ewk_Url_Response *response; /**< resource load response */
};

/// Creates a type name for _Ewk_Web_Resource_Load_Error.
typedef struct _Ewk_Web_Resource_Load_Error Ewk_Web_Resource_Load_Error;

/**
 * @brief Structure containing details about a resource load error.
 *
 * Details given about a resource load failure.
 */
struct _Ewk_Web_Resource_Load_Error {
    Ewk_Web_Resource *resource; /**< resource that failed loading */
    Ewk_Web_Error *error; /**< load error */
};

/**
 * Creates a new EFL WebKit view object.
 *
 * @param e canvas object where to create the view object
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_add(Evas *e);

/**
 * Creates a new EFL WebKit view object based on specific Ewk_Context.
 *
 * @param e canvas object where to create the view object
 * @param context Ewk_Context object to declare process model
 *
 * @return view object on success or @c NULL on failure
 */
EAPI Evas_Object *ewk_view_add_with_context(Evas *e, Ewk_Context *context);

/**
 * Asks the object to load the given URI.
 *
 * @param o view object to load @a URI
 * @param uri uniform resource identifier to load
 *
 * @return @c EINA_TRUE is returned if @a o is valid, irrespective of load.
 */
EAPI Eina_Bool ewk_view_uri_set(Evas_Object *o, const char *uri);

/**
 * Returns the current URI string of view object.
 *
 * It returns an internal string and should not
 * be modified. The string is guaranteed to be stringshared.
 *
 * @param o view object to get current URI
 *
 * @return current URI on success or @c NULL on failure
 */
EAPI const char *ewk_view_uri_get(const Evas_Object *o);

/**
 * Asks the main frame to reload the current document.
 *
 * @param o view object to reload current document
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 *
 * @see ewk_view_reload_bypass_cache()
 */
EAPI Eina_Bool    ewk_view_reload(Evas_Object *o);

/**
 * Reloads the current page's document without cache.
 *
 * @param o view object to reload current document
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_reload_bypass_cache(Evas_Object *o);

/**
 * Asks the main frame to stop loading.
 *
 * @param o view object to stop loading
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool    ewk_view_stop(Evas_Object *o);

/**
 * Delivers a Web intent to the view's main frame.
 *
 * @param o view object to deliver the intent to
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool    ewk_view_intent_deliver(Evas_Object *o, Ewk_Intent *intent);

/**
 * Asks the main frame to navigate back in the history.
 *
 * @param o view object to navigate back
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 *
 * @see ewk_frame_back()
 */
EAPI Eina_Bool    ewk_view_back(Evas_Object *o);

/**
 * Asks the main frame to navigate forward in the history.
 *
 * @param o view object to navigate forward
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE otherwise
 *
 * @see ewk_frame_forward()
 */
EAPI Eina_Bool    ewk_view_forward(Evas_Object *o);

/**
 * Queries if it is possible to navigate backwards one item in the history.
 *
 * @param o view object to query if backward navigation is possible
 *
 * @return @c EINA_TRUE if it is possible to navigate backwards in the history, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool    ewk_view_back_possible(Evas_Object *o);

/**
 * Queries if it is possible to navigate forwards one item in the history.
 *
 * @param o view object to query if forward navigation is possible
 *
 * @return @c EINA_TRUE if it is possible to navigate forwards in the history, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool    ewk_view_forward_possible(Evas_Object *o);

/**
 * Gets the current title of the main frame.
 *
 * It returns an internal string and should not
 * be modified. The string is guaranteed to be stringshared.
 *
 * @param o view object to get current title
 *
 * @return current title on success or @c NULL on failure
 */
EAPI const char *ewk_view_title_get(const Evas_Object *o);

/**
 * Gets the current load progress of page.
 *
 * The progress estimation from 0.0 to 1.0.
 *
 * @param o view object to get the current progress
 *
 * @return the load progres of page, value from 0.0 to 1.0.
 */
EAPI double ewk_view_load_progress_get(const Evas_Object *o);

/**
 * Loads the specified @a html string as the content of the view.
 *
 * External objects such as stylesheets or images referenced in the HTML
 * document are located relative to @a baseUrl.
 *
 * If an @a unreachableUrl is passed it is used as the url for the loaded
 * content. This is typically used to display error pages for a failed
 * load.
 *
 * @param o view object to load the HTML into
 * @param html HTML data to load
 * @param baseUrl Base URL used for relative paths to external objects (optional)
 * @param unreachableUrl URL that could not be reached (optional)
 *
 * @return @c EINA_TRUE if it the HTML was successfully loaded, @c EINA_FALSE otherwise
 */
EAPI Eina_Bool ewk_view_html_string_load(Evas_Object *o, const char *html, const char *baseUrl, const char *unreachableUrl);

/**
 * Queries the ratio between the CSS units and device pixels when the content is unscaled.
 *
 * When designing touch-friendly contents, knowing the approximated target size on a device
 * is important for contents providers in order to get the intented layout and element
 * sizes.
 *
 * As most first generation touch devices had a PPI of approximately 160, this became a
 * de-facto value, when used in conjunction with the viewport meta tag.
 *
 * Devices with a higher PPI learning towards 240 or 320, applies a pre-scaling on all
 * content, of either 1.5 or 2.0, not affecting the CSS scale or pinch zooming.
 *
 * This value can be set using this property and it is exposed to CSS media queries using
 * the -webkit-device-pixel-ratio query.
 *
 * For instance, if you want to load an image without having it upscaled on a web view
 * using a device pixel ratio of 2.0 it can be done by loading an image of say 100x100
 * pixels but showing it at half the size.
 *
 * @media (-webkit-min-device-pixel-ratio: 1.5) {
 *     .icon {
 *         width: 50px;
 *         height: 50px;
 *         url: "/images/icon@2x.png"; // This is actually a 100x100 image
 *     }
 * }
 *
 * If the above is used on a device with device pixel ratio of 1.5, it will be scaled
 * down but still provide a better looking image.
 *
 * @param o view object to get device pixel ratio
 *
 * @return the ratio between the CSS units and device pixels.
 */
EAPI float ewk_view_device_pixel_ratio_get(const Evas_Object *o);

/**
 * Sets the ratio between the CSS units and device pixels when the content is unscaled.
 *
 * @param o view object to set device pixel ratio
 *
 * @return @c EINA_TRUE if the device pixel ratio was set, @c EINA_FALSE otherwise
 *
 * @see ewk_view_device_pixel_ratio_get()
 */
EAPI Eina_Bool ewk_view_device_pixel_ratio_set(Evas_Object *o, float ratio);

/**
 * Sets the theme path that will be used by this view.
 *
 * This also sets the theme on the main frame. As frames inherit theme
 * from their parent, this will have all frames with unset theme to
 * use this one.
 *
 * @param o view object to change theme
 * @param path theme path, may be @c NULL to reset to the default theme
 */
EAPI void ewk_view_theme_set(Evas_Object *o, const char *path);

/**
 * Gets the theme set on this view.
 *
 * This returns the value set by ewk_view_theme_set().
 *
 * @param o view object to get theme path
 *
 * @return the theme path, may be @c NULL if not set
 */
EAPI const char *ewk_view_theme_get(const Evas_Object *o);

/**
 * Gets the current custom character encoding name.
 *
 * @param o view object to get the current encoding
 *
 * @return @c eina_strinshare containing the current encoding, or
 *         @c NULL if it's not set
 */
EAPI const char  *ewk_view_setting_encoding_custom_get(const Evas_Object *o);

/**
 * Sets the custom character encoding and reloads the page.
 *
 * @param o view to set the encoding
 * @param encoding the new encoding to set or @c NULL to restore the default one
 *
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise
 */
EAPI Eina_Bool    ewk_view_setting_encoding_custom_set(Evas_Object *o, const char *encoding);

#ifdef __cplusplus
}
#endif
#endif // ewk_view_h
