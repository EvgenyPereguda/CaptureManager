﻿#pragma checksum "..\..\PositionControl.xaml" "{8829d00f-11b8-4213-878b-770e8597ac16}" "1B45B4B2342B524F561C970DCAFA9B34AB05CA87CA8A89D8F8E2A254FD939A43"
//------------------------------------------------------------------------------
// <auto-generated>
//     Этот код создан программой.
//     Исполняемая версия:4.0.30319.42000
//
//     Изменения в этом файле могут привести к неправильной работе и будут потеряны в случае
//     повторной генерации кода.
// </auto-generated>
//------------------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;


namespace WPFWebViewerEVRDisplayAsync {
    
    
    /// <summary>
    /// PositionControl
    /// </summary>
    public partial class PositionControl : System.Windows.Controls.UserControl, System.Windows.Markup.IComponentConnector {
        
        
        #line 11 "..\..\PositionControl.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Border m_PositionerRect;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/WPFWebViewerEVRDisplayAsync;component/positioncontrol.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\PositionControl.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Maintainability", "CA1502:AvoidExcessiveComplexity")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            this.m_PositionerRect = ((System.Windows.Controls.Border)(target));
            
            #line 11 "..\..\PositionControl.xaml"
            this.m_PositionerRect.MouseEnter += new System.Windows.Input.MouseEventHandler(this.m_PositionerRect_MouseLeave);
            
            #line default
            #line hidden
            
            #line 11 "..\..\PositionControl.xaml"
            this.m_PositionerRect.MouseLeave += new System.Windows.Input.MouseEventHandler(this.m_PositionerRect_MouseLeave);
            
            #line default
            #line hidden
            
            #line 11 "..\..\PositionControl.xaml"
            this.m_PositionerRect.MouseMove += new System.Windows.Input.MouseEventHandler(this.m_PositionerRect_MouseMove);
            
            #line default
            #line hidden
            
            #line 11 "..\..\PositionControl.xaml"
            this.m_PositionerRect.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.m_PositionerRect_MouseLeftButtonUp);
            
            #line default
            #line hidden
            
            #line 11 "..\..\PositionControl.xaml"
            this.m_PositionerRect.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.m_PositionerRect_MouseLeftButtonDown);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}

