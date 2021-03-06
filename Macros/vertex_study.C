#include "Plotter.h"

void vertex_study()
{
  //TFile f( "Samples/output_Run2016BCG_looseCuts_9may.root" );
  TFile f( "Samples/output_Run2016BCG_looseCuts_28jun.root" );
  TTree* t = (TTree*)f.Get( "ntp" );
  const unsigned short max_diph = 50;
  unsigned int num_diph;
  t->SetBranchAddress( "num_diphoton", &num_diph );
  float diphoton_m[max_diph];
  t->SetBranchAddress( "diphoton_mass", diphoton_m );
  float pho1_e[max_diph], pho2_e[max_diph];
  t->SetBranchAddress( "diphoton_energy1", pho1_e );
  t->SetBranchAddress( "diphoton_energy2", pho2_e );
  float pho1_sc_x[max_diph], pho1_sc_y[max_diph], pho1_sc_z[max_diph];
  float pho2_sc_x[max_diph], pho2_sc_y[max_diph], pho2_sc_z[max_diph];
  t->SetBranchAddress( "diphoton_supercluster_x1", pho1_sc_x );
  t->SetBranchAddress( "diphoton_supercluster_y1", pho1_sc_y );
  t->SetBranchAddress( "diphoton_supercluster_z1", pho1_sc_z );
  t->SetBranchAddress( "diphoton_supercluster_x2", pho2_sc_x );
  t->SetBranchAddress( "diphoton_supercluster_y2", pho2_sc_y );
  t->SetBranchAddress( "diphoton_supercluster_z2", pho2_sc_z );
  float bs_x0, bs_y0, bs_z0;
  t->SetBranchAddress( "bs_x0", &bs_x0 );
  t->SetBranchAddress( "bs_y0", &bs_y0 );
  t->SetBranchAddress( "bs_z0", &bs_z0 );

  TH1D* h_vtx_reco_m = new TH1D( "vtx_reco_m", "m(#gamma#gamma)@@Events@@GeV", 80, 300., 1900. ),
    *h_vtx_bs_m = (TH1D*)h_vtx_reco_m->Clone( "vtx_bs_m" ),
    *h_vtx_orig_m = (TH1D*)h_vtx_reco_m->Clone( "vtx_orig_m" );
  TH1D* h_mdiff_bs = new TH1D( "mdiff_bs", "#Deltam(#gamma#gamma)@@Events@@GeV?.2f", 200, -25., 25. ),
    *h_mdiff_orig = (TH1D*)h_mdiff_bs->Clone( "mdiff_orig" ),
    *h_mdiff_bs_orig = (TH1D*)h_mdiff_bs->Clone( "mdiff_bs_orig" );

  const unsigned short num_vtx_pos = 21;
  const float min_vtx_dist = -10., max_vtx_dist = 10.;
  float vtx_shifts[num_vtx_pos];
  TH1D* h_mdiff_shift[num_vtx_pos];
  for ( unsigned short i=0; i<num_vtx_pos; i++ ) {
    vtx_shifts[i] = min_vtx_dist+( max_vtx_dist-min_vtx_dist )*i/( num_vtx_pos-1 );
    h_mdiff_shift[i] = new TH1D( Form( "mdiff_shift_%d", i ), "#Deltam(#gamma#gamma)@@Events@@GeV?.2f", 200, -25., 25. );
    //cout << "vtx shift " << i << " = " << vtx_shifts[i] << endl;
  }

  //----- events analysis

  for ( unsigned long long i=0; i<t->GetEntries(); i++ ) {
    t->GetEntry( i );
    const TVector3 bs_pos( bs_x0, bs_y0, bs_z0 );
    for ( unsigned short j=0; j<num_diph; j++ ) {
      const TVector3 pho1_sc( pho1_sc_x[j], pho1_sc_y[j], pho1_sc_z[j] );
      const TVector3 pho2_sc( pho2_sc_x[j], pho2_sc_y[j], pho2_sc_z[j] );
      TVector3 p1 = ( pho1_sc-bs_pos ).Unit() * pho1_e[j];
      TVector3 p2 = ( pho2_sc-bs_pos ).Unit() * pho2_e[j];
      TLorentzVector corr_p1( p1.x(), p1.y(), p1.z(), pho1_e[j] );
      TLorentzVector corr_p2( p2.x(), p2.y(), p2.z(), pho2_e[j] );
      TVector3 p1_orig = ( pho1_sc ).Unit() * pho1_e[j];
      TVector3 p2_orig = ( pho2_sc ).Unit() * pho2_e[j];
      TLorentzVector corr_p1_orig( p1_orig.x(), p1_orig.y(), p1_orig.z(), pho1_e[j] );
      TLorentzVector corr_p2_orig( p2_orig.x(), p2_orig.y(), p2_orig.z(), pho2_e[j] );
      h_vtx_reco_m->Fill( diphoton_m[j] );
      h_vtx_bs_m->Fill( ( corr_p1+corr_p2 ).M() );
      h_vtx_orig_m->Fill( ( corr_p1_orig+corr_p2_orig ).M() );
      h_mdiff_bs->Fill( ( corr_p1+corr_p2 ).M()-diphoton_m[j] );
      h_mdiff_orig->Fill( ( corr_p1_orig+corr_p2_orig ).M()-diphoton_m[j] );
      h_mdiff_bs_orig->Fill( ( corr_p1_orig+corr_p2_orig ).M()-( corr_p1+corr_p2 ).M() );
      for ( unsigned short k=0; k<num_vtx_pos; k++ ) {
//cout << k << ": " << vtx_shifts[i]
        const TVector3 shift_pos( 0., 0., vtx_shifts[k] );
        TVector3 p1_shift = ( pho1_sc-shift_pos ).Unit() * pho1_e[j];
        TVector3 p2_shift = ( pho2_sc-shift_pos ).Unit() * pho2_e[j];
        TLorentzVector corr_p1_shift( p1_shift.x(), p1_shift.y(), p1_shift.z(), pho1_e[j] );
        TLorentzVector corr_p2_shift( p2_shift.x(), p2_shift.y(), p2_shift.z(), pho2_e[j] );
        h_mdiff_shift[k]->Fill( ( corr_p1_shift+corr_p2_shift ).M()-diphoton_m[j] );
      }
    }
  }

cout << "finished the events loop" << endl;
  TGraphErrors gr_mdiff;
  for ( unsigned short i=0; i<num_vtx_pos; i++ ) {
    gr_mdiff.SetPoint( i, vtx_shifts[i], h_mdiff_shift[i]->GetMean() );
    //gr_mdiff.SetPointError( i, 0., h_mdiff_shift[i]->GetRMS() );
    //gr_mdiff.SetPointError( i, 0., h_mdiff_shift[i]->GetStdDev() );
    //cout << "mean mass difference for z=" << vtx_shifts[i] << ": " << h_mdiff_shift[i]->GetMean() << endl;
  }

  //----- plotting part

  const float lumi = 9.412003739742 * 1.e3; // pre-TS2 runs with pots inserted
  const char* top_label = Form( "CMS Preliminary 2016, #sqrt{s} = 13 TeV, L = %.1f fb^{-1}", lumi/1.e3 );
  gStyle->SetOptStat( 0 );
  {
    Canvas c( "mreco_dist", top_label );
    h_mdiff_bs->Draw( "hist" );
    h_mdiff_bs->SetLineColor( kBlack );
    h_mdiff_bs->SetLineWidth( 2 );
    h_mdiff_bs->SetMaximum( 3.e4 );
    c.SetLegendX1( 0.15 );
    c.SetLegendY1( 0.78 );
    c.AddLegendEntry( h_mdiff_bs, "BS - reco vtx" );
    h_mdiff_orig->Draw( "hist same" );
    h_mdiff_orig->SetLineColor( kRed+1 );
    h_mdiff_orig->SetLineWidth( 2 );
    h_mdiff_orig->SetLineStyle( 2 );
    c.AddLegendEntry( h_mdiff_orig, "(0, 0, 0) - reco vtx" );
    h_mdiff_bs_orig->Draw( "hist same" );
    h_mdiff_bs_orig->SetLineColor( kBlue+1 );
    h_mdiff_bs_orig->SetLineWidth( 2 );
    h_mdiff_bs_orig->SetLineStyle( 4 );
    c.AddLegendEntry( h_mdiff_bs_orig, "(0, 0, 0) - BS" );
    c.Prettify( h_mdiff_bs );
    c.SetLogy();
    c.SetGrid();
    c.Save( "pdf,png", "~lforthom/www/private/twophoton/vtx_study" );
  }

  {
    Canvas c( "mreco_vs_vtxz", top_label );
    gr_mdiff.SetTitle( "Vertex z (cm)@@m(#gamma#gamma) at given z - m(#gamma#gamma) at #gamma#gamma vertex z (GeV)" );
    gr_mdiff.Draw( "ap" );
    gr_mdiff.SetMarkerStyle( 20 );
    c.Prettify( gr_mdiff.GetHistogram() );
    c.Save( "pdf,png", "~lforthom/www/private/twophoton/vtx_study" );
  }

  Plotter plt( "~lforthom/www/private/twophoton/vtx_study", top_label );
  {
    Plotter::HistsMap hm;
    hm.emplace_back( "Reco. vertex", h_vtx_reco_m );
    hm.emplace_back( "Beam spot", h_vtx_bs_m );
    hm.emplace_back( "Vertex at (0, 0, 0)", h_vtx_orig_m );
    plt.plot_multihists( "mreco_comparison", hm, 0.24, 1.76 );
  }
}
