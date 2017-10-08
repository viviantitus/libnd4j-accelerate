//
// Based on PyTorch - https://github.com/pytorch/pytorch
//

#ifndef LIBND4J_CONVOLUTIONS_H
#define LIBND4J_CONVOLUTIONS_H

#include <NDArray.h>

namespace nd4j {
    namespace ops {

        Nd4jIndex convsize(Nd4jIndex x, Nd4jIndex k, Nd4jIndex s, const char* vf);

        template<typename T>
        Nd4jStatus conv3D(T* output_data,
                    T alpha,
                    T* ptr_input, Nd4jIndex nInputDepth, Nd4jIndex nInputRows, Nd4jIndex nInputCols,
                    T* ptr_weight, Nd4jIndex nKernelDepth, Nd4jIndex nKernelRows, Nd4jIndex nKernelCols,
                    Nd4jIndex sdepth, Nd4jIndex srow, Nd4jIndex scol,
                    const char *vf, const char *xc);

        template<typename T>
        Nd4jStatus conv3Dmv(NDArray<T>* r_, T beta, T alpha, NDArray<T>* t_, NDArray<T>* k_, Nd4jIndex sdepth, Nd4jIndex srow, Nd4jIndex scol, const char *vf, const char *xc);


        template<typename T>
        void fullXCorr3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc);

        template<typename T>
        void fullConv3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc);

        template<typename T>
        void validXCorr3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc);

        template<typename T>
        void validConv3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc);

        template<typename T>
        void _dilatedMaxPool3D(T *input_p, T *output_p, T *indz_p, Nd4jIndex nslices, Nd4jIndex itime, Nd4jIndex iwidth, Nd4jIndex iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int kT, int kW, int kH, int dT, int dW, int dH, int pT, int pW, int pH, int dilationT, int dilationW, int dilationH);

        template<typename T>
        void _dilatedMaxPool3D_bp(T *gradInput_p, T *gradOutput_p, T *indz_p, Nd4jIndex nslices, Nd4jIndex  itime, Nd4jIndex  iwidth, Nd4jIndex  iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int dT, int dW, int dH, int pT, int pW, int pH, int dilationT, int dilationW, int dilationH);

        template<typename T>
        void _avgPool3D(T *input_p, T *output_p, Nd4jIndex nslices, Nd4jIndex itime, Nd4jIndex iwidth, Nd4jIndex iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int kT, int kW, int kH, int dT, int dW, int dH, int padT, int padW, int padH, bool count_include_pad);

        template<typename T>
        void _avgPool3D_bp(T *gradInput_p, T *gradOutput_p, Nd4jIndex nslices, Nd4jIndex itime, Nd4jIndex iwidth, Nd4jIndex iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int kT, int kW, int kH, int dT, int dW, int dH, int padT, int padW, int padH, bool count_include_pad);

        template <typename T>
        void _vol2col(const T *data_vol, const int channels, const int depth, const int height, const int width, const int kT, const int kH, const int kW, const int pT, const int pH, const int pW, const int dT, const int dH, const int dW, const int dilationT, const int dilationH, const int dilationW, T *data_col);

        template <typename T>
        void _col2vol(const T* data_col, const int channels, const int depth, const int height, const int width, const int out_depth, const int out_height, const int out_width, const int kT, const int kH, const int kW, const int pT, const int pH, const int pW, const int dT, const int dH, const int dW, const int dilationT, const int dilationH, const int dilationW, T* data_vol);

		void calcOutHWpool2D(int& oH, int& oW, const int kH, const int kW, const int sH, const int sW, const int pH, const int pW, const int dH, const int dW, const int iH, const int iW, const int isSameMode);

        void _calcPadding2D(int& pH, int& pW, int oH, int oW, int inH, int inW, int kH, int kW, int sH, int sW, int dH, int dW);

        template <typename T>
        void _im2col(const T* data_im, const int channels,
        const int height, const int width, const int kernel_h, const int kernel_w,
        const int pad_h, const int pad_w,
        const int stride_h, const int stride_w,
        const int dilation_h, const int dilation_w,
                T* data_col);
    }
}

template <typename T>
void nd4j::ops::_im2col(const T* data_im, const int channels,
             const int height, const int width, const int kernel_h, const int kernel_w,
             const int pad_h, const int pad_w,
             const int stride_h, const int stride_w,
             const int dilation_h, const int dilation_w,
             T* data_col) {
    const int height_col = (height + 2 * pad_h - (dilation_h * (kernel_h - 1) + 1)) / stride_h + 1;
    const int width_col = (width + 2 * pad_w - (dilation_w * (kernel_w - 1) + 1)) / stride_w + 1;
    const int channels_col = channels * kernel_h * kernel_w;

    for (int c_col = 0; c_col < channels_col; ++c_col) {
        int w_offset = c_col % kernel_w;
        int h_offset = (c_col / kernel_w) % kernel_h;
        int c_im = c_col / kernel_h / kernel_w;

        for (int h_col = 0; h_col < height_col; ++h_col) {
            for (int w_col = 0; w_col < width_col; ++w_col) {
                int h_im = h_col * stride_h - pad_h + h_offset * dilation_h;
                int w_im = w_col * stride_w - pad_w + w_offset * dilation_w;

                data_col[(c_col * height_col + h_col) * width_col + w_col] = (h_im >= (int) 0 && w_im >= (int) 0 && h_im < height &&
                                                                              w_im < width) ?
                                                                             data_im[(c_im * height + h_im) * width +
                                                                                     w_im] : (T) 0.f;
            }
        }
    }
}

void nd4j::ops::_calcPadding2D(int& pH, int& pW, int oH, int oW, int inH, int inW, int kH, int kW, int sH, int sW, int dH, int dW) {
    int eKH, eKW;

    if (dH == 1 && dW == 1) {
        eKH = kH;
        eKW = kW;
    } else {
        eKH = kH + (kH - 1) * (dH - 1);
        eKW = kW + (kW - 1) * (dW - 1);
    }

    pH = ((oH - 1) * sH + eKH - inH) / 2; //Note that padBottom is 1 bigger than this if bracketed term is not divisible by 2
    pW = ((oW - 1) * sW + eKW - inW) / 2;
}

template <typename T>
void nd4j::ops::_vol2col(const T *data_vol, const int channels, const int depth, const int height, const int width, const int kT, const int kH, const int kW, const int pT, const int pH, const int pW, const int dT, const int dH, const int dW, const int dilationT, const int dilationH, const int dilationW, T *data_col) {
    int c, t, h, w;
    int depth_col  = (depth  + 2 * pT - (dilationT * (kT - 1) + 1)) / dT + 1;
    int height_col = (height + 2 * pH - (dilationH * (kH - 1) + 1)) / dH + 1;
    int width_col  = (width  + 2 * pW - (dilationW * (kW - 1) + 1)) / dW + 1;
    int channels_col = channels * kT * kH * kW;
    for (c = 0; c < channels_col; ++c)
    {
        int w_offset = c % kW;
        int h_offset = (c / kW) % kH;
        int t_offset = (c / kW / kH) % kT;
        int c_vol = c / kT / kH / kW;
        for (t = 0; t < depth_col; ++t)
        {
            for (h = 0; h < height_col; ++h)
            {
                for (w = 0; w < width_col; ++w)
                {
                    int t_pad = t * dT - pT + t_offset * dilationT;
                    int h_pad = h * dH - pH + h_offset * dilationH;
                    int w_pad = w * dW - pW + w_offset * dilationW;
                    if (t_pad >= 0 && t_pad < depth &&
                        h_pad >= 0 && h_pad < height &&
                        w_pad >= 0 && w_pad < width)
                        data_col[((c * depth_col + t) * height_col + h) * width_col + w] =
                                data_vol[((c_vol * depth + t_pad) * height + h_pad) * width + w_pad];
                    else
                        data_col[((c * depth_col + t) * height_col + h) * width_col + w] = 0;
                }
            }
        }
    }
}

template <typename T>
void nd4j::ops::_col2vol(const T* data_col, const int channels, const int depth, const int height, const int width, const int out_depth, const int out_height, const int out_width, const int kT, const int kH, const int kW, const int pT, const int pH, const int pW, const int dT, const int dH, const int dW, const int dilationT, const int dilationH, const int dilationW, T* data_vol) {
    int c, t, h, w;
    memset(data_vol, 0, sizeof(T) * depth * height * width * channels);
    int depth_col  = out_depth;
    int height_col = out_height;
    int width_col  = out_width;
    int channels_col = channels * kT * kH * kW;
    for (c = 0; c < channels_col; ++c)
    {
        int w_offset = c % kW;
        int h_offset = (c / kW) % kH;
        int t_offset = (c / kW / kH) % kT;
        int c_vol = c / kT / kH / kW;
        for (t = 0; t < depth_col; ++t)
        {
            for (h = 0; h < height_col; ++h)
            {
                for (w = 0; w < width_col; ++w)
                {
                    int t_pad = t * dT - pT + t_offset * dilationT;
                    int h_pad = h * dH - pH + h_offset * dilationH;
                    int w_pad = w * dW - pW + w_offset * dilationW;
                    if (t_pad >= 0 && t_pad < depth &&
                        h_pad >= 0 && h_pad < height &&
                        w_pad >= 0 && w_pad < width)
                        data_vol[((c_vol * depth + t_pad) * height + h_pad) * width + w_pad] +=
                                data_col[((c * depth_col + t) * height_col + h) * width_col + w];
                }
            }
        }
    }
}

template<typename T>
void nd4j::ops::_avgPool3D_bp(T *gradInput_p, T *gradOutput_p, Nd4jIndex nslices, Nd4jIndex itime, Nd4jIndex iwidth, Nd4jIndex iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int kT, int kW, int kH, int dT, int dW, int dH, int padT, int padW, int padH, bool count_include_pad) {
    for (int k = 0; k < nslices; k++)
    {
        Nd4jIndex i, j, ti;

        /* local pointers */
        T *ip = gradInput_p + k * itime * iwidth * iheight;
        T *op = gradOutput_p + k * otime * owidth * oheight;
        for (i = 0; i < itime*iwidth*iheight; i++)
            *(ip + i) = 0;

        /* loop over output */
        for (ti = 0; ti < otime; ti++)
        {
            for (i = 0; i < oheight; i++)
            {
                for (j = 0; j < owidth; j++)
                {
                    Nd4jIndex tstart = ti * dT - padT;
                    Nd4jIndex hstart = i  * dH - padH;
                    Nd4jIndex wstart = j  * dW - padW;
                    Nd4jIndex tend = nd4j::math::nd4j_min<Nd4jIndex>(tstart + kT, itime + padT);
                    Nd4jIndex hend = nd4j::math::nd4j_min<Nd4jIndex>(hstart + kH, iheight + padH);
                    Nd4jIndex wend = nd4j::math::nd4j_min<Nd4jIndex>(wstart + kW, iwidth + padW);
                    Nd4jIndex pool_size = (tend -tstart) * (hend - hstart) * (wend - wstart);
                    tstart = nd4j::math::nd4j_max<Nd4jIndex>(tstart, 0);
                    hstart = nd4j::math::nd4j_max<Nd4jIndex>(hstart, 0);
                    wstart = nd4j::math::nd4j_max<Nd4jIndex>(wstart, 0);
                    tend = nd4j::math::nd4j_min<Nd4jIndex>(tend, itime);
                    hend = nd4j::math::nd4j_min<Nd4jIndex>(hend, iheight);
                    wend = nd4j::math::nd4j_min<Nd4jIndex>(wend, iwidth);

                    Nd4jIndex divide_factor;
                    if (count_include_pad)
                        divide_factor = pool_size;
                    else
                        divide_factor = (tend - tstart) * (hend - hstart) * (wend - wstart);

                    /* scatter gradients out to footprint: */
                    T val  = *op++;

                    long x,y,z;
                    for (z = tstart; z < tend; z++)
                    {
                        for (y = hstart; y < hend; y++)
                        {
                            for (x = wstart; x < wend; x++)
                            {
                                *(ip + z * iheight * iwidth + y * iwidth + x) += val / divide_factor;
                            }
                        }
                    }
                }
            }
        }
    }
}

template<typename T>
void nd4j::ops::_avgPool3D(T *input_p, T *output_p, Nd4jIndex nslices, Nd4jIndex itime, Nd4jIndex iwidth, Nd4jIndex iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int kT, int kW, int kH, int dT, int dW, int dH, int padT, int padW, int padH, bool count_include_pad) {
    for (Nd4jIndex k = 0; k < nslices; k++)
    {
        long i, j, ti;

        /* local pointers. */
        T *ip = input_p + k * itime * iwidth * iheight;
        T *op = output_p + k * otime * owidth * oheight;
        for (i = 0; i < otime * oheight * owidth; ++i)
            *(op + i) = 0;

        /* loop over output */
        for (ti = 0; ti < otime; ti++)
        {
            for (i = 0; i < oheight; i++)
            {
                for (j = 0; j < owidth; j++)
                {
                    /* compute pool range. */
                    Nd4jIndex tstart = ti * dT - padT;
                    Nd4jIndex hstart = i  * dH - padH;
                    Nd4jIndex wstart = j  * dW - padW;
                    Nd4jIndex tend = nd4j::math::nd4j_min<Nd4jIndex>(tstart + kT, itime + padT);
                    Nd4jIndex hend = nd4j::math::nd4j_min<Nd4jIndex>(hstart + kH, iheight + padH);
                    Nd4jIndex wend = nd4j::math::nd4j_min<Nd4jIndex>(wstart + kW, iwidth + padW);
                    Nd4jIndex pool_size = (tend - tstart) * (hend - hstart) * (wend - wstart);
                    tstart = nd4j::math::nd4j_max<Nd4jIndex>(tstart, 0);
                    hstart = nd4j::math::nd4j_max<Nd4jIndex>(hstart, 0);
                    wstart = nd4j::math::nd4j_max<Nd4jIndex>(wstart, 0);
                    tend = nd4j::math::nd4j_min<Nd4jIndex>(tend, itime);
                    hend = nd4j::math::nd4j_min<Nd4jIndex>(hend, iheight);
                    wend = nd4j::math::nd4j_min<Nd4jIndex>(wend, iwidth);

                    Nd4jIndex divide_factor;
                    if (count_include_pad)
                        divide_factor = pool_size;
                    else
                        divide_factor = (tend - tstart) * (hend - hstart) * (wend - wstart);

                    /* compute local sum: */
                    T sum = (T) 0.0f;
                    long x, y, z;

                    for (z = tstart; z < tend; z++)
                    {
                        for (y = hstart; y < hend; y++)
                        {
                            for (x = wstart; x < wend; x++)
                            {
                                sum +=  *(ip + z * iwidth * iheight + y * iwidth + x);
                            }
                        }
                    }

                    /* set output to local max */
                    *op++ += sum / divide_factor;
                }
            }
        }
    }
}


template<typename T>
void nd4j::ops::_dilatedMaxPool3D_bp(T *gradInput_p, T *gradOutput_p, T *indz_p, Nd4jIndex nslices, Nd4jIndex  itime, Nd4jIndex  iwidth, Nd4jIndex  iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int dT, int dW, int dH, int pT, int pW, int pH, int dilationT, int dilationW, int dilationH) {
    for (int k = 0; k < nslices; k++)
    {
        T *gradInput_p_k  = gradInput_p  + k * itime * iwidth * iheight;
        T *gradOutput_p_k = gradOutput_p + k * otime * owidth * oheight;
        T *indz_p_k = indz_p + k * otime * owidth * oheight;

        /* calculate max points */
        long ti, i, j;
        for (ti = 0; ti < otime; ti++)
        {
            for (i = 0; i < oheight; i++)
            {
                for (j = 0; j < owidth; j++)
                {
                    /* retrieve position of max */
                    T * indzp = &indz_p_k[ti * oheight * owidth + i * owidth + j];
                    long maxti = ((unsigned char*)(indzp))[0] * dilationT + ti * dT - pT;
                    long maxi  = ((unsigned char*)(indzp))[1] * dilationH + i * dH - pH;
                    long maxj  = ((unsigned char*)(indzp))[2] * dilationW + j * dW - pW;

                    if (maxti != -1) {
                        /* update gradient */
                        gradInput_p_k[maxti * iheight * iwidth + maxi * iwidth + maxj] += gradOutput_p_k[ti * oheight * owidth + i * owidth + j];
                    }
                }
            }
        }
    }
}

template<typename T>
void nd4j::ops::_dilatedMaxPool3D(T *input_p, T *output_p, T *indz_p, Nd4jIndex nslices, Nd4jIndex itime, Nd4jIndex iwidth, Nd4jIndex iheight, Nd4jIndex otime, Nd4jIndex owidth, Nd4jIndex oheight, int kT, int kW, int kH, int dT, int dW, int dH, int pT, int pW, int pH, int dilationT, int dilationW, int dilationH) {
    Nd4jIndex k;
//#pragma omp parallel for private(k)
    for (k = 0; k < nslices; k++)
    {
        /* loop over output */
        Nd4jIndex i, j, ti;
        for (ti = 0; ti < otime; ti++)
        {
            for (i = 0; i < oheight; i++)
            {
                for (j = 0; j < owidth; j++)
                {
                    /* local pointers */

                    Nd4jIndex start_t = ti * dT - pT;
                    Nd4jIndex start_h = i * dH - pH;
                    Nd4jIndex start_w = j * dW - pW;

                    Nd4jIndex kernel_t = nd4j::math::nd4j_min<Nd4jIndex>(kT, kT + start_t);
                    Nd4jIndex kernel_h = nd4j::math::nd4j_min<Nd4jIndex>(kH, kH + start_h);
                    Nd4jIndex kernel_w = nd4j::math::nd4j_min<Nd4jIndex>(kW, kW + start_w);

                    while(start_t < 0)
                        start_t += dilationT;
                    while(start_h < 0)
                        start_h += dilationH;
                    while(start_w < 0)
                        start_w += dilationW;

                    T *ip = input_p + k * itime * iwidth * iheight + start_t * iwidth * iheight + start_h * iwidth + start_w;
                    T *op = output_p + k * otime * owidth * oheight + ti * owidth * oheight + i * owidth + j;
                    T *indzp = indz_p + k * otime * owidth * oheight + ti * owidth * oheight + i * owidth + j;

                    /* compute local max: */
                    T maxval = -MAX_FLOAT;
                    int x,y,z;
                    int mx, my, mz;
                    mx = my = mz = -1;

                    for (z = 0; z < kernel_t; z++)
                    {
                        for (y = 0; y < kernel_h; y++)
                        {
                            for (x = 0; x < kernel_w; x++)
                            {
                                if ((start_t + z * dilationT < itime) && (start_h + y * dilationH < iheight) && (start_w + x * dilationW < iwidth))
                                {
                                    T val = *(ip + z * dilationT * iwidth * iheight + y * dilationH * iwidth + x * dilationW);
                                    if (val > maxval)
                                    {
                                        maxval = val;
                                        // Store indices w.r.t the kernel dimension
                                        mz = z + (kT - kernel_t);
                                        my = y + (kH - kernel_h);
                                        mx = x + (kW - kernel_w);
                                    }
                                }
                            }
                        }
                    }

                    // set max values
                    ((unsigned char*)(indzp))[0] = mz;
                    ((unsigned char*)(indzp))[1] = my;
                    ((unsigned char*)(indzp))[2] = mx;
                    ((unsigned char*)(indzp))[3] = 0;

                    /* set output to local max */
                    *op = maxval;
                }
            }
        }
    }
}

template<typename T>
void nd4j::ops::validXCorr3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc) {
    Nd4jIndex tot = (it - kt) / st + 1;
    Nd4jIndex tor = (ir - kr) / sr + 1;
    Nd4jIndex toc = (ic - kc) / sc + 1;

    Nd4jIndex zz, xx, yy;

    for (zz = 0; zz < tot; zz++) {
        for(yy = 0; yy < tor; yy++) {
            for(xx = 0; xx < toc; xx++) {
                /* Dot product in two dimensions... (between input image and the mask) */
                T *pi_ = t_ + zz*st*ir*ic + yy*sr*ic + xx*sc;
                T *pw_ = k_;
                T sum = 0;
                Nd4jIndex kz, kx, ky;
                for(kz = 0; kz < kt; kz++) {
                    for(ky = 0; ky < kr; ky++) {
                        for(kx = 0; kx < kc; kx++) {
                            sum += pi_[kx]*pw_[kx];
                        }
                        pi_ += ic; /* next input line */
                        pw_ += kc; /* next mask line */
                    }
                    pi_ += (ir-kr)*ic; /* next input slice */
                }
                /* Update output */
                *r_++ += sum*alpha;
            }
        }
    }
}

template<typename T>
void nd4j::ops::validConv3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc) {
    Nd4jIndex tot = (it - kt) / st + 1;
    Nd4jIndex tor = (ir - kr) / sr + 1;
    Nd4jIndex toc = (ic - kc) / sc + 1;

    Nd4jIndex zz, xx, yy;

    for(zz = 0; zz < tot; zz++) {
        for(yy = 0; yy < tor; yy++) {
            for(xx = 0; xx < toc; xx++) {
                /* Dot product in two dimensions... (between input image and the mask) */
                T *pi_ = t_ + zz*st*ir*ic + yy*sr*ic + xx*sc;
                T *pw_ = k_ + kt*kr*kc - 1;
                T sum = 0;
                Nd4jIndex kz, kx, ky;
                for(kz = 0; kz < kt; kz++) {
                    for(ky = 0; ky < kr; ky++) {
                        for(kx = 0; kx < kc; kx++) {
                            sum += pi_[kx]*pw_[-kx];
                        }
                        pi_ += ic; /* next input line */
                        pw_ -= kc; /* next mask line */
                    }
                    pi_ += (ir-kr)*ic; /* next input slice */
                }
                /* Update output */
                *r_++ += alpha*sum;
            }
        }
    }
}

template<typename T>
void nd4j::ops::fullConv3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc) {
    Nd4jIndex tor = (ir - 1) * sr + kr;
    Nd4jIndex toc = (ic - 1) * sc + kc;

    Nd4jIndex zz, xx, yy;

    for(zz = 0; zz < it; zz++) {
        for(yy = 0; yy < ir; yy++) {
            for(xx = 0; xx < ic; xx++) {
                /* Outer product in two dimensions... (between input image and the mask) */
                T *po_ = r_ + zz*st*tor*toc + yy*sr*toc + xx*sc;
                T *pw_ = k_;
                Nd4jIndex kz, kx, ky;
                /* printf("Output Plane : %ld,%ld,%ld, input val=%g\n",zz,yy,xx,*t_); */
                for(kz = 0; kz < kt; kz++) {
                    for(ky = 0; ky < kr; ky++) {
                        T z = *t_ * alpha;
                        for(kx = 0; kx < kc; kx++) {
                            /* printf("o=%g,k=%g," , po_[kx],pw_[kx]); */
                            po_[kx] += z * pw_[kx];
                            /* printf("o=%g " , po_[kx]); */
                        }
                        /* printf("\n"); */
                        po_ += toc; /* next input line */
                        pw_ += kc; /* next mask line */
                    }
                    po_ += (tor-kr)*toc; /* next output slice */
                    /* printf("\n"); */
                }
                t_++;
            }
        }
    }
}

template<typename T>
void nd4j::ops::fullXCorr3Dptr(T*r_, T alpha, T *t_, Nd4jIndex it, Nd4jIndex ir, Nd4jIndex ic, T *k_, Nd4jIndex kt, Nd4jIndex kr, Nd4jIndex kc, Nd4jIndex st, Nd4jIndex sr, Nd4jIndex sc) {
    Nd4jIndex tor = (ir - 1) * sr + kr;
    Nd4jIndex toc = (ic - 1) * sc + kc;

    Nd4jIndex zz, xx, yy;

    for(zz = 0; zz < it; zz++) {
        for(yy = 0; yy < ir; yy++) {
            for(xx = 0; xx < ic; xx++) {
                /* Outer product in two dimensions... (between input image and the mask) */
                T *po_ = r_ + zz * st * tor * toc + yy*sr*toc + xx*sc;
                T *pw_ = k_ + kt*kr*kc -1;
                Nd4jIndex kz, kx, ky;
                for(kz = 0; kz < kt; kz++) {
                    for(ky = 0; ky < kr; ky++) {
                        T z = *t_ * alpha;
                        for(kx = 0; kx < kc; kx++) {
                            po_[kx] += z * pw_[-kx];
                        }
                        po_ += toc; /* next input line */
                        pw_ -= kc; /* next mask line */
                    }
                    po_ += (tor-kr)*toc; /* next output slice */
                }
                t_++;
            }
        }
    }
}

Nd4jIndex nd4j::ops::convsize(Nd4jIndex x, Nd4jIndex k, Nd4jIndex s, const char* vf) {
    if (*vf == 'V')
        return (x-k)/s + 1;
    else
        return (x-1)*s + k;
}

template<typename T>
Nd4jStatus nd4j::ops::conv3Dmv(NDArray<T>* r_, T beta, T alpha, NDArray<T>* t_, NDArray<T>* k_,
              Nd4jIndex sdepth, Nd4jIndex srow, Nd4jIndex scol, const char *vf, const char *xc) {

    Nd4jIndex nInputPlane, nInputDepth, nInputRows, nInputCols;
    Nd4jIndex nKernelDepth, nKernelRows, nKernelCols;
    Nd4jIndex nOutputPlane, nOutputDepth, nOutputRows, nOutputCols;
    Nd4jIndex istride0, kstride0, kstride1;
    NDArray<T> *input;
    NDArray<T> *kernel;
    T* input_data;
    T* weight_data;
    T* output_data;
    Nd4jIndex nelem;
    Nd4jIndex k, i;

    if (t_->rankOf() != 4)
        throw "Boom";
        //return ND4J_STATUS_BAD_DIMENSIONS;

    if (k_->rankOf() != 5)
        throw "Boom";
        //return ND4J_STATUS_BAD_DIMENSIONS;

    if (sdepth < 1 || srow < 1 || scol < 1)
        throw "Boom";
        //return ND4J_STATUS_BAD_PARAMS;

    if (!(*vf == 'V' || *vf == 'F'))
        throw "Boom";
        //return ND4J_STATUS_BAD_PARAMS;

    if (!(*xc == 'X' || *xc == 'C'))
        throw "Boom";
        //return ND4J_STATUS_BAD_PARAMS;

    input = t_->isContiguous() ? t_ : t_->dup(t_->ordering());
    if (!(k_->stridesOf()[4] == 1 || k_->stridesOf()[3] == k_->sizeAt(4))) {
        kernel = k_->isContiguous() ? k_ : k_->dup(k_->ordering());
    } else {
        kernel = k_;
    }


    nInputPlane = input->sizeAt(0);
    istride0    = input->stridesOf()[0];
    nInputDepth = input->sizeAt(1);
    nInputRows  = input->sizeAt(2);
    nInputCols  = input->sizeAt(3);

    kstride0    = kernel->stridesOf()[0];
    kstride1    = kernel->stridesOf()[1];
    nKernelDepth = kernel->sizeAt(2);
    nKernelRows = kernel->sizeAt(3);
    nKernelCols = kernel->sizeAt(4);
    nOutputPlane = kernel->sizeAt(0);

    if (kernel->sizeAt(1) != nInputPlane)
        throw "Boom";
        //return ND4J_STATUS_BAD_DIMENSIONS;


    if (!((nInputDepth >= nKernelDepth && nInputRows >= nKernelRows && nInputCols >= nKernelCols) || *vf == 'F'))
        throw "Boom";
        //return ND4J_STATUS_BAD_PARAMS;

    nOutputDepth = convsize(nInputDepth, nKernelDepth, sdepth, vf);
    nOutputRows = convsize(nInputRows, nKernelRows, srow, vf);
    nOutputCols = convsize(nInputCols, nKernelCols, scol, vf);

    nelem = r_->lengthOf();

    if (r_->sizeAt(0) != nOutputPlane || r_->sizeAt(1) != nOutputDepth || r_->sizeAt(2) != nOutputRows || r_->sizeAt(3)!= nOutputCols) {
        nd4j_printf("Failed at r_ size: {%i, %i, %i, %i} vs {}", r_->sizeAt(0), r_->sizeAt(1), r_->sizeAt(2), r_->sizeAt(3), nOutputPlane, nOutputDepth, nOutputRows, nOutputCols);
        throw "Boom";
        //return ND4J_STATUS_BAD_DIMENSIONS;
    }

    if (nelem == 0 || beta == (T) 0.0f || nelem != r_->lengthOf()) {
        r_->assign((T) 0.0f);
    }
    else if (beta != (T) 1.0f) // stupid comparison
        r_->template applyScalar<simdOps::Multiply<T>>(beta);


    input_data = input->getBuffer();
    weight_data = kernel->getBuffer();
    output_data = r_->getBuffer();

    for(k = 0; k < nOutputPlane; k++) {
        for(i = 0; i < nInputPlane; i++) {
            /* get kernel */
            T* ptr_weight = weight_data + k*kstride0 + i*kstride1;
            /* get input */
            T* ptr_input = input_data + i*istride0;

            /* do image, kernel convolution */
            conv3D(output_data,
                              alpha,
                              ptr_input,  nInputDepth, nInputRows,  nInputCols,
                              ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                              sdepth, srow, scol, vf, xc);
        }
        /* Next output plane */
        output_data += nOutputDepth*nOutputCols*nOutputRows;
    }


    return ND4J_STATUS_OK;
}

template<typename T>
Nd4jStatus nd4j::ops::conv3D(T* output_data,
            T alpha,
            T* ptr_input, Nd4jIndex nInputDepth, Nd4jIndex nInputRows, Nd4jIndex nInputCols,
            T* ptr_weight, Nd4jIndex nKernelDepth, Nd4jIndex nKernelRows, Nd4jIndex nKernelCols,
            Nd4jIndex sdepth, Nd4jIndex srow, Nd4jIndex scol,
            const char *vf, const char *xc) {

    if (!(*vf == 'V' || *vf == 'F'))
        return ND4J_STATUS_BAD_PARAMS;

    if (!(*xc == 'X' || *xc == 'C'))
        return ND4J_STATUS_BAD_PARAMS;


    if (*vf == 'F')
        if (*xc == 'X') {
            nd4j::ops::fullXCorr3Dptr<T>(output_data,
                           alpha,
                           ptr_input, nInputDepth, nInputRows,  nInputCols,
                           ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                           sdepth, srow, scol);
        } else {
            nd4j::ops::fullConv3Dptr<T>(output_data,
                          alpha,
                          ptr_input, nInputDepth, nInputRows,  nInputCols,
                          ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                          sdepth, srow, scol);
        }
    else
        if (*xc == 'X') {
            nd4j::ops::validXCorr3Dptr<T>(output_data,
                            alpha,
                            ptr_input, nInputDepth, nInputRows,  nInputCols,
                            ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                            sdepth, srow, scol);
        } else {
            nd4j::ops::validConv3Dptr<T>(output_data,
                           alpha,
                           ptr_input, nInputDepth, nInputRows,  nInputCols,
                           ptr_weight, nKernelDepth, nKernelRows, nKernelCols,
                           sdepth, srow, scol);
        }

    return ND4J_STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
// calculation of output height and width during 2D pooling procedure
void nd4j::ops::calcOutHWpool2D(int& oH, int& oW, const int kH, const int kW, const int sH, const int sW, const int pH, const int pW, const int dH, const int dW, const int iH, const int iW, const int isSameMode) {
	if(isSameMode > 0) {
		oH = (int) nd4j::math::nd4j_ceil(iH * 1.f / sH);
		oW = (int) nd4j::math::nd4j_ceil(iW * 1.f / sW);
	}
	else {
		oH = (iH - kH - (kH-1)*(dH-1) + 2*pH)/sH + 1;
		oW = (iW - kW - (kW-1)*(dW-1) + 2*pW)/sW + 1;
	}
}

#endif //LIBND4J_CONVOLUTIONS_H
