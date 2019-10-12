//                                  
//                                  
//                       file:   App.h
//                                  


#pragma once

#include "pch.h"
#include "Common\DeviceResources.h" 
#include "GVMain.h"



namespace VHG 
{
    //                  Main entry point...
    //                                               
    //       The "App" class connects the application 
    //       to the Windows shell 
    //       and handles application lifecycle events.
    //                                               


    ref class App sealed : public Windows::ApplicationModel::Core::IFrameworkView
    {
    
    public:
    
        App();


        //          IFrameworkView Methods

        virtual void Initialize(
                Windows::ApplicationModel::Core::CoreApplicationView^ applicationView
        ); 

        virtual void SetWindow(Windows::UI::Core::CoreWindow^ window); 

        virtual void Load(Platform::String^ entryPoint);
        
        virtual void Run();
       
        virtual void Uninitialize();


    protected:

        // Application lifecycle event handlers:



        void OnActivated(
                Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, 
                Windows::ApplicationModel::Activation::IActivatedEventArgs^ args
        );



        void OnSuspending(
                Platform::Object^ sender, 
                Windows::ApplicationModel::SuspendingEventArgs^ args
        );



        void OnResuming(
                Platform::Object^ sender, 
                Platform::Object^ args
        );



        // Window event handlers.
        
        
        void OnWindowSizeChanged(
                Windows::UI::Core::CoreWindow^ sender, 
                Windows::UI::Core::WindowSizeChangedEventArgs^ args
        );
      

       
        void OnVisibilityChanged(
                Windows::UI::Core::CoreWindow^ sender, 
                Windows::UI::Core::VisibilityChangedEventArgs^ args
        );
     

      
        void OnWindowClosed(
                Windows::UI::Core::CoreWindow^ sender, 
                Windows::UI::Core::CoreWindowEventArgs^ args
        );

     
     
  
        // DisplayInformation event handlers.
       
       
        void OnDpiChanged(
                Windows::Graphics::Display::DisplayInformation^ sender, 
                Platform::Object^ args
        );
        
       

        void OnOrientationChanged(
                Windows::Graphics::Display::DisplayInformation^ sender, 
                Platform::Object^ args
        );
        
       

        void OnDisplayContentsInvalidated(
                Windows::Graphics::Display::DisplayInformation^ sender, 
                Platform::Object^ args
        );

    private:

        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        //   
        //   The "App" class will hold a unique pointer to VHG::GVMain: 
        //                  

        std::unique_ptr<VHG::GVMain> m_main;

        bool m_windowClosed;
   
        bool m_windowVisible;
    
    };  //  Closes ref class "App"; 

}  //  Closes namespace VHG;


ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
    virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};


